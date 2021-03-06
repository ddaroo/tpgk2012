#include "stdafx.h"
#include "Serwer.h"
#include "StopWatch.h"
#include "Dictionary.h"
#include "PlayerDrivers.h"

string toString(boost::optional<int> limit)
{
	if(limit)
		return lexical_cast<string>(*limit);
	else
		return "<no limit>";
}

CSerwer::CSerwer(void)
{
	if(vm["initTime"].as<unsigned>())
		timeLimitForInit = vm["initTime"].as<unsigned>();
	if(vm["moveTime"].as<unsigned>())
		timeLimitForMove = vm["moveTime"].as<unsigned>();

	LOGFL("Time for init is %s miliseconds, time for move %s miliseconds", toString(timeLimitForInit) % toString(timeLimitForMove));

    FOREACH(const auto &ps, gs.players)
	{
		LOGFL("Creating player #%d...", ps.ID);
		const int socketPlayersCount = vm["connections"].as<unsigned>();
		assert(socketPlayersCount <= (int) gs.players.size());

		auto &driver = playerDrivers[ps.ID];
		if(ps.ID < socketPlayersCount) 
			driver = make_unique<JavaPlayer>("ScrabPlayer.jar");
		else 
			driver = make_unique<CppDummyPlayer>();

		realizeWithTimeLimit(timeLimitForInit, [&]
		{
			CHECK_TIME_FORMAT("\tTime spent by player %d on initialization: ", ps.ID);
			driver->init(gs.rules);
		});

		driver->playerName = playerNames[ps.ID] + driver->playerName + " (" + typeid(*driver).name() + ")";
	}

	LOGL("Finished setting up players. They are:");
	FOREACH(const auto &pd, playerDrivers)
	{
		LOGFL("\t%d. %s", pd.first % pd.second->playerName);
	}
}

CSerwer::~CSerwer(void)
{
}

void CSerwer::run()
{
	do
	{
		const int playerToMove = gs.nextMove();
		const CPlayerState &ps = gs.players[playerToMove];
		if(ps.disqualified)
		{
			LOGFL("Ignoring player %d who is disqualified.", playerToMove);
			continue;
		}

		try
		{
			TAction action;
			realizeWithTimeLimit(timeLimitForMove, [&]
			{
				try
				{
					CHECK_TIME_FORMAT("Player %d was thinking", playerToMove);
					action = playerDrivers[playerToMove]->takeAction(gs.board, ps);
				}
				catch(std::exception &e)
				{
					disqualifyPlayer(playerToMove, e.what());
				}
			});
			LOGFL("Player %d attempts an action of type %s", playerToMove % action.type().name());
			gs.applyAction(action);
		}
		catch(TimeLimitExceededException &tlee)
		{
			LOGFL("Player %d violated time limit: %s.", playerToMove % tlee.what());
			LOGFL("Because violation isn't grave, it will be forgiven. Player %d loses turn.", playerToMove);
			gs.applyAction(SkipTurn());
		}
		catch(std::exception &e)
		{
			LOGFL("Encountered an exception during player %d turn: %s", playerToMove % e.what());
			if(vm.count("lenient"))
			{
				LOGFL("We're in a lenient mode, so this will be forgiven. Player %d loses turn.", playerToMove);
				gs.applyAction(SkipTurn());
			}
			else
			{
				LOGL("It's unforgivable. Player will be disqualified.");
				disqualifyPlayer(playerToMove, string("Unforgivable violation: ") + e.what());
			}
		}

		gs.handleEndingCondition();
	} while(!gs.isGameFinished());
	
	for(auto it = playerDrivers.begin(); it != playerDrivers.end(); ++it) 
	{
		try
		{
			(*it).second->gameFinished();
		}
		catch(std::exception &e)
		{
			LOGFL("Player %d failed to handle ending of the game: %s", it->first % e.what());
		}
	}

	LOGL("Game results:");
	FOREACH(auto player, gs.players)
	{
		LOGFL("\tPlayer %d - %d points.", player.ID % player.points);
	}
}



void CSerwer::realizeWithTimeLimit(boost::optional<int> timeLimit, function<void()> action)
{
	if(!timeLimit)
	{
		action();
	}
	else
	{
		bool timeLimitExceeded = false;
		int limitInMs = *timeLimit;
		int timePassed = -1;

		boost::thread t([&]() -> bool
		{
			CStopWatch timer;
			action();
			timePassed = timer.getDiff();
			if(timePassed > limitInMs)
			{
				LOGFL("Time limit exceeded: used time was %d ms while the limit was %d ms!", timePassed % limitInMs);
				timeLimitExceeded = true;
			}

			return true;
		});

		CStopWatch mainThreadWatch;
		if(!t.timed_join(boost::posix_time::milliseconds(limitInMs * 2)))
		{
			//watek sie nie skonczyl mimo dlugiego czekania
			LOGFL("We've been waiting for %d ms (while the limit was %d) but the program is still not responding. Totally unacceptable! Server will close.", mainThreadWatch.getDiff() % limitInMs);
			logger << flush;
			disqualifyPlayer(gs.activePlayer, "Unforgivable time limit violation!");
			throw std::runtime_error("Player did not give any answer");
			//exit(-1);
		}

		if(timeLimitExceeded)
			throw TimeLimitExceededException(limitInMs, timePassed);
	}
}

void CSerwer::disqualifyPlayer(int id, string reason /*= ""*/)
{
	string name;
	if(playerDrivers.count(id))
		name = playerDrivers[id]->playerName;
	else 
		name = playerNames[id];

	LOGFL("Disqualification! Reason: %s", reason);
	LOGFL("Player %d is disqualified! Game ends for him! Shame on %s!", id % name);

	gs.players[id].disqualified = true;
	gs.players[id].disqualificationReason = reason;
}

CBoard::CBoard()
{
	tiles.resize(WIDTH);
	FOREACH(auto &col, tiles)
		col.resize(HEIGHT);

	foreachPos([this](Pos p)
	{
		CTile &t = tile(p);
		t.pos = p;
	});


	//////////////////////////////////////////////////////////////////////////
	// UKLADANIE BONUSOWYCH POL	
	////////////////////////////////////////////////////////////////////////// 

	
	CTile::Bonus twiceWord(CTile::Bonus::WORD, 1), 
		tripleWord(CTile::Bonus::WORD, 2), 
		twiceLetter(CTile::Bonus::LETTER, 1),
		tripleLetter(CTile::Bonus::LETTER, 2);


	//pomocnicza funkcja - ustawia symetrycznie cztery pola na raz
	auto setSymmetrically = [this](int x, int y, CTile::Bonus bonus)
	{
		Pos p1, p2, p3, p4;
		p1 = Pos(x, y);
		p2 = Pos(WIDTH-1-x,  y);
		p3 = Pos(x,  HEIGHT-1-y);
		p4 = Pos(WIDTH-1-x,  HEIGHT-1-y);;
		tile(p1).bonus = tile(p2).bonus = tile(p3).bonus = tile(p4).bonus = bonus;
	};

	//uklad bonusów wg http://hjkgfg.fm.interia.pl/Scrabble-plansza-rys.bmp

	//przekatne
	setSymmetrically(0, 0, tripleWord);
	setSymmetrically(1, 1, twiceWord);
	setSymmetrically(2, 2, twiceWord);
	setSymmetrically(3, 3, twiceWord);
	setSymmetrically(4, 4, twiceWord);
	setSymmetrically(5, 5, tripleLetter);
	setSymmetrically(6, 6, twiceLetter);
	setSymmetrically(7, 7, twiceWord);
	//premie nad przekatna
	setSymmetrically(0, 3, twiceLetter);
	setSymmetrically(0, 7, tripleWord);
	setSymmetrically(1, 5, tripleLetter);
	setSymmetrically(2, 6, twiceLetter);
	setSymmetrically(2, 7, twiceLetter);
	//premie pod przekatna
	setSymmetrically(3, 0, twiceLetter);
	setSymmetrically(7, 0, tripleWord);
	setSymmetrically(5, 1, tripleLetter);
	setSymmetrically(6, 2, twiceLetter);
	setSymmetrically(7, 2, twiceLetter);
}

void CBoard::foreachPos(std::function<void(Pos)> func) const
{
	for (int i = 0; i < WIDTH ; i++)
		for (int j = 0; j < HEIGHT ; j++)
			func(Pos(i,j));
}

CTile & CBoard::tile(Pos p)
{
	return tiles[p.x][p.y];
}

const CTile & CBoard::tile(Pos p) const
{
	return tiles[p.x][p.y];
}

void CTile::writeData(boost::asio::ip::tcp::socket& sock) const
{
	pos.writeData(sock);
	char a = letter.get_value_or('@');
	uint16_t bonVal = htons(bonus.multiplicatorValue);
	write(sock, boost::asio::buffer(&a, 1));
	write(sock, boost::asio::buffer(&bonus.multiplicatorType, 1));
	write(sock, boost::asio::buffer(&bonVal, 2));
}

void CBoard::foreachTile(function<void(const CTile &)> func) const
{
	for (int j = 0; j < HEIGHT ; j++)
		for (int i = 0; i < WIDTH ; i++)
			func(tiles[i][j]);
}

void CBoard::writeData(boost::asio::ip::tcp::socket& sock) const
{
	uint16_t dimension = htons(tiles.size());
	int written = write(sock, boost::asio::buffer(&dimension, 2));
	dimension = htons(tiles.front().size());
	written = write(sock, boost::asio::buffer(&dimension, 2));
	CTile tile;
	FOREACH(auto &col, tiles) 
	{
		FOREACH(auto &tile, col) 
		{
			tile.writeData(sock);
		}
	}
}

vector<string> CBoard::wordsGeneratedbyLetter(char letter, Pos pos) const
{
	vector<string> ret;
	FOREACH(auto orientation, orientations)
	{
		string generatedWord = wordGeneratedbyLetter(letter, pos, orientation);
		if(generatedWord.size() > 1)
			ret.push_back(move(generatedWord));
	}

	return ret;
}

string CBoard::wordGeneratedbyLetter(char letter, Pos pos, EOrientation orientation) const
{
	//kawalki slowa przed i po kladzionej literze (moga wyjsc puste)
	const string textBefore = lettersNextTo(pos, orientation, false);
	const string textAfter = lettersNextTo(pos, orientation, true);

	return textBefore + letter + textAfter;
}

Pos CBoard::lastLetterInDirection(Pos src, Pos dir) const
{
	do 
	{
		src += dir;
	} while (src.isValid() && tile(src).letter);

	src += -dir; //we went one tile to far...
	return src;
}

 string CBoard::readWord(Pos from, Pos dir) const
 {
 	string ret;
 	while(from.isValid())
 	{
 		const CTile &t = tile(from);
 		if(t.letter)
 			ret.push_back(*t.letter);
 		else
 			break;
 
 		from += dir;
 	}
 
 	return ret;
 }

string CBoard::lettersNextTo(Pos pos, EOrientation orientation, bool forward) const
{
	const Pos direction = (forward ? forwardDirection : backwardDirection)(orientation);

	string ret;
	while(1)
	{
		pos += direction;
		if(pos.isValid() && tile(pos).letter)
			ret += *tile(pos).letter;
		else
			break;
	}

	//jesli czytalismy slowo wstecz, odwrocmy wynik
	if(!forward)
		reverse(ret);

	return ret;
}

WordRange CBoard::getWordRange(Pos pos, EOrientation orientation)
{
	return WordRange(
		lastLetterInDirection(pos, backwardDirection(orientation)),
		lastLetterInDirection(pos, forwardDirection(orientation))		);
}

Pos CBoard::centerTile() const
{
	return Pos(WIDTH / 2, HEIGHT / 2); 
}

bool CBoard::empty() const
{
	for (int j = 0; j < HEIGHT ; j++)
		for (int i = 0; i < WIDTH ; i++)
			if(tiles[i][j].letter)
				return false;

	return true;
}

CGameState::CGameState()
{
	activePlayer = turn = -1;
	createLetters();

	const int playersCount = vm["players"].as<unsigned>();
	LOGFL("There will be %d players participating in game.", playersCount);
	LOGFL("%d of them will be handled through TCP socket.", vm["connections"].as<unsigned>());

	for (int i = 0; i < playersCount; i++)
		addNewPlayer();
}

void CGameState::createLetters()
{
	ifstream lettersInfo("conf_letters.txt");
	if(!lettersInfo)
		MY_THROW("Critical error: Cannot open conf_letters.txt");

	while(true)
	{
		istringstream line;
		string temp;
		getline(lettersInfo, temp);
		if(!lettersInfo)
			break;

		if(temp.size() > 2 && temp[0] == '/')
			continue;

		line.str(temp);
		char letter;
		int points, count;
		line >> letter >> count >> points;

		if(!line)
			break;

		rules.pointsForLetter[letter] = points;
		for (int i = 0; i < count ; i++)
			letters.push_back(letter);
	}

	LOGFL("Successfully prepared %d letters.", letters.size());
}

void CGameState::addNewPlayer()
{
	players.push_back(CPlayerState(players.size()));
}

void CGameState::startGame()
{
	//losujemy poczatkowe litery dla graczy
	random_shuffle(letters);
	FOREACH(auto &ps, players)
		drawLetters(ps);
}

void moveLettersFromBegin(vector<CLetter> &src, unsigned count, vector<CLetter> &out)
{
	assert(src.size() >= count);
	copy_n(src.begin(), count, back_inserter(out));
	src.erase(src.begin(), src.begin() + count);
}

void CGameState::drawLetters(CPlayerState &ps, unsigned toWhatCount)
{
	assert(toWhatCount > ps.letters.size()); //nie mozna wolac metody, gdy gracz ma za duzo liter (bo sie unsigned przekreci)
	unsigned howMany = toWhatCount - ps.letters.size();
	amin(howMany, letters.size());
	if(howMany)
		moveLettersFromBegin(letters, howMany, ps.letters);
	else
		LOGL("We've run out of letters!");
}

int CGameState::nextMove()
{
	if(turn < 0)
	{
		startGame();
		turn = 0;
		activePlayer = 0;
		LOGL("First turn started.");
	}
	else
	{
		activePlayer++;
		if(activePlayer >= players.size())
		{
			activePlayer %= players.size();
			turn++;
			LOGFL("Turn %d started.", turn);
		}
	}

	print();
	return activePlayer;
}

bool CGameState::isGameFinished() const
{
	return result;
}

//polimorfizm bez polimorfizmu :>
struct ActionApplier : boost::static_visitor<>
{
	ActionApplier(CGameState &_gs, CPlayerState &_ps) : gs(_gs), ps(_ps) {}
	CGameState &gs;
	CPlayerState &ps;
	template<typename T> void operator()(const T &pl)
	{
		if(!gs.validateAction(pl, ps))
		{
			LOGFL("Player %d attempts an illegal action!!!", ps.ID);
			//TODO - obsluzyc jakos lagodniej, niz wywalac program
			throw runtime_error("Illegal action attempted!");
		}
		LOGL("Action has been correctly validated.");
		gs.applyAction(pl);
	}
};

void CGameState::applyAction(const TAction &action)
{
	//wywola nam applyAction z odpowiednia akcja w argumencie
	auto actap = ActionApplier(*this, players[activePlayer]);
	action.apply_visitor(actap);
}

struct PointsForWordsAccumulator
{
private:
	int pointsForLetters;
	int wordPointsMultiplier;

public:
	PointsForWordsAccumulator()
	{
		pointsForLetters = 0;
		wordPointsMultiplier = 1;
	}

	void addLetter(const CTile &t, int basicLetterValue)
	{
		//no points for blank
		if(t.letterGeneratedFromBlank)
			basicLetterValue = 0;

		int multiplierForThisLetter = 1;

		if(!t.bonus.usedUp)
		{
			switch(t.bonus.multiplicatorType)
			{
			case CTile::Bonus::WORD:
				wordPointsMultiplier += t.bonus.multiplicatorValue;
				break;
			case CTile::Bonus::LETTER:
				multiplierForThisLetter += t.bonus.multiplicatorValue;
				break;
			default:
				break;
			}
		}

		pointsForLetters += multiplierForThisLetter * basicLetterValue;
	}

	int totalPoints() const
	{
		return pointsForLetters * wordPointsMultiplier;
	}
};

void CGameState::applyAction(const PutLetters &action)
{
	CPlayerState &ps = players[activePlayer];

	LOGFL("Applying action: Player %d puts %d letters on board:", ps.ID % action.letters.size());
	FOREACH(auto &pl, action.letters)
		LOGF("\t%c at %s,\t", pl.letter % pl.pos);
	LOG("\n");

	//wykladamy litery
	FOREACH(auto &pl, action.letters)
	{
		auto &t = board.tile(pl.pos);
		t.letterGeneratedFromBlank = pl.generateFromBlank;
		t.letter = pl.letter;
		ps.letters -= pl.generateFromBlank ? BLANK : pl.letter;
	}

	//litery leza na planszy, mozemy naliczac punkty
	if(ps.letters.empty())
	{
		LOGFL("Awarding player with %d points for using all his letters.", BONUS_FOR_EMPTYING_RACK);
		ps.points += BONUS_FOR_EMPTYING_RACK;
	}
	
	WordRange mainWord = board.getWordRange(action.letters.front().pos, action.orientation);
	ps.points += pointsForWord(mainWord); //punkty za "glowne slowo
	LOGFL("Awarding player with %d points for the main word: \"%s\"", pointsForWord(mainWord) % board.readWord(mainWord.first, forwardDirection(mainWord.getOrientation())));

	 EOrientation prostopadlyKierunek = flip(action.orientation);
	 FOREACH(auto &pl, action.letters)
	 {
		 WordRange perpendicularWord = board.getWordRange(pl.pos, prostopadlyKierunek);
		 if(perpendicularWord.length() > 1)
		 {
			 ps.points += pointsForWord(perpendicularWord);
			 LOGFL("Awarding player with %d points for perpendicular word: \"%s\"", pointsForWord(perpendicularWord) % board.readWord(perpendicularWord.first, forwardDirection(perpendicularWord.getOrientation())));
		 }
	 }

	 //wykladamy litery
	 FOREACH(auto &pl, action.letters)
	 {
		 auto &t = board.tile(pl.pos);
		 if(t.bonus.multiplicatorType != CTile::Bonus::NONE)
		 {
			 LOGFL("The bonus on tile %s has been used up.", pl.pos);
			 t.bonus.usedUp = true;
		 }
	 }


	//dobierz liter, zeby miec tyle, ile na starcie
	drawLetters(ps);
	ps.turnsSkipped = 0;
}

void CGameState::applyAction(const SkipTurn &action)
{
	CPlayerState &ps = players[activePlayer];
	ps.turnsSkipped++;
}

void CGameState::applyAction(const ExchangeLetters &action)
{
	CPlayerState &ps = players[activePlayer];
	LOGFL("Applying action: Player %d exchanges %d letters: %s", ps.ID % action.letters.size() % formatLetters(action.letters));
	LOGFL("\tPlayer letters before exchange: %s", formatLetters(ps.letters));

	ps.exchanges++;
	FOREACH(auto c, action.letters)
	{
		ps.letters -= c;
		letters.push_back(c);
	}

	random_shuffle(letters);
	drawLetters(ps);

	LOGFL("\tPlayer letters after exchange: %s", formatLetters(ps.letters));
	LOGFL("\tCount of exchanges done: %d", ps.exchanges);
}

void CGameState::print() const
{
	logger << "\t |";
	for (int i = 0; i < WIDTH ; i++)
		logger << intToChar(i);
	logger << "\n\t-+";
	for (int i = 0; i < WIDTH ; i++)
		logger << '-';
	logger << '\n';

	for (int j = 0; j < HEIGHT ; j++)
	{
		logger << "\t" << intToChar(j) << "|";
		for (int i = 0; i < WIDTH ; i++)
		{
			const auto &l = board.tiles[i][j].letter;
			if(l)
				logger << *l;
			else
				logger << '.';
		}
		logger << "\n";
	}

	LOGFL("Letters in the main bag: %s", formatLetters(letters));
	LOGL("Players in game:");
	FOREACH(const auto &ps, players)
		LOGFL("\t%d - %d points, available letters: %s", ps.ID % ps.points % formatLetters(ps.letters));

	LOGFL("\nCurrent player: %d", activePlayer);
}

bool CGameState::validateAction(const PutLetters &action, const CPlayerState &ps) const
{
	LOGFL("Player %d decided to put %d letters on board:", ps.ID % action.letters.size());
	for(int i = 0; i < (int)action.letters.size(); i++)
		LOGFL("\t%d. Letter '%c' at %s.", i % action.letters[i].letter % action.letters[i].pos);
	LOGL("\n\nAction will be validated now.");


	{
		//sprawdzamy, czy gracz ma te litery  -> kopiujemy jego zapas i iterujemy po kladzionych literach, usuwajac je z zapasu
		auto availableLeters = ps.letters; 
		FOREACH(auto pl, action.letters)
		{
			CLetter letterToRemove = pl.generateFromBlank ? BLANK : pl.letter;
			auto letterItr = find(availableLeters, letterToRemove);
			if(letterItr != availableLeters.end())
				availableLeters.erase(letterItr);
			else
			{
				LOGFL("Player is trying to put other letters than owned! (missing '%c')", letterToRemove);
				return false;
			}
		}
	}
	if(action.orientation == INVALID) //pionowo albo poziomo, nie inaczej
	{
		LOGL("Put letters request declared an invalid direction!");
		return false;
	}

	if(action.letters.empty()) //jak kladziemy, to kladziemy
	{
		LOGL("Put letters request doesn't contain any letters to put!");
		return false;
	}

	if(board.empty())
	{
		Pos center = board.centerTile();
		auto itr = find_if(action.letters, [=](PutLetters::PutLetter pl)
		{
			return pl.pos == center;
		});

		if(itr == action.letters.end())
		{
			LOGL("First word put on the board has to go through the middle tile!");
			return false;
		}
	}


	string wordCenter; //kadlub slowa powstajacy miedzy skrajnymi literami po ich polzoeniu na planszy

	for(int i = 0; i < (int)action.letters.size() - 1; i++)
	{
		//dla kazdej pary sasiadujacych liter
		auto &a = action.letters[i], 
			&b = action.letters[i+1]; 

		if(!a.pos.isValid() || !b.pos.isValid())
		{
			LOGL("An attempt to put letter beyond board limits!");
			return false;
		}
		if(orientationOfPos(a.pos, b.pos) != action.orientation) //jesli litery nie leza na zadanej prostej (pion/poziom)
		{
			LOGL("The letters do not lie on the declared line!");
			return false;
		}
		if(!(a.pos.x < b.pos.x || a.pos.y < b.pos.y)) //jesli nie ida po kolei
		{
			LOGL("The letters are in wrong order!");
			return false;
		}

		wordCenter += a.letter;

		Pos forward = forwardDirection(action.orientation);
		for(Pos pos = a.pos + forward; pos != b.pos; pos += forward) 
		{
			//dla kazdej pozycji miedzy kladzionymi literami -> na planszy musi byc litera
			if(!board.tile(pos).letter)
			{
				LOGFL("There is an empty tile at %s between letters %c and %c", pos % a.letter % b.letter);
				return false;
			}

			wordCenter += *board.tile(pos).letter; 
		}
	}

	int perpendicularWords = 0;
	FOREACH(auto &ps, action.letters)
	{
		string perpendicularWord = board.wordGeneratedbyLetter(ps.letter, ps.pos, flip(action.orientation));
		if(perpendicularWord.size() > 1)
		{
			LOGF("Perpendicular word created: \"%s\". ", perpendicularWord);
			perpendicularWords++;
			if(dictionary->contains(perpendicularWord))
				LOGL("It's valid.");
			else
			{
				LOGL("It's invalid!");
				return false;
			}

		}
	}

	wordCenter += action.letters.back().letter; //dodajemy ostatnia litera bo petla jest po parach (do size - 1)

	const string beforeWord = board.lettersNextTo(action.letters.front().pos, action.orientation, false),
		afterWord = board.lettersNextTo(action.letters.back().pos, action.orientation, true);

	const string wholeWord = beforeWord + wordCenter + afterWord;
	LOGF("Player attempts to create a word: \"%s\". ", wholeWord);
	if(dictionary->contains(wholeWord))
		LOGL("It's valid.");
	else
	{
		LOGL("It's invalid!");
		return false;
	}

	//slowo musi laczyc sie z lezacymi na planszy
	//mozliwosci spelnienia
	//1) wewnatrz kladzionych liter jest dziura wypelniona lezaca na planszy litera
	//2) slowo idzie rownolegle, przylegle do innego (tworzy slowa prostopadle)
	//3) slowo bezposrednio przed lub po sobie ma lezace na planszy litery
	if(!board.empty()  &&  !perpendicularWords)
	{
		Pos p = action.letters.front().pos, 
			afterLast = action.letters.back().pos + forwardDirection(action.orientation);

		bool atLeastOneCommonLetter = false;

		while(p != afterLast)
		{
			if(board.tile(p).letter)
			{
				atLeastOneCommonLetter = true;
				break;
			}
			p += forwardDirection(action.orientation);
		}

		if(!atLeastOneCommonLetter)
		{
			Pos tileBefore = action.letters.front().pos + backwardDirection(action.orientation),
				tileAfter = action.letters.back().pos + forwardDirection(action.orientation);

			bool beforeFirst = tileBefore.isValid() && board.tile(tileBefore).letter;
			bool afterLast = tileAfter.isValid() && board.tile(tileAfter).letter;

			if(!beforeFirst && !afterLast)
			{
				LOGL("Players tries to put a word that is not adjoining words already present on board!");
				return false;
			}
		}
	}

	return true;
}

bool CGameState::validateAction(const SkipTurn &action, const CPlayerState &ps) const
{
	return true; //one can always skip a turn
}

bool CGameState::validateAction(const ExchangeLetters &action, const CPlayerState &ps) const
{
	LOGFL("Player %d decided to exchange %d letters: %s", ps.ID % action.letters.size() % formatLetters(action.letters));

	if(action.letters.empty())
	{
		LOGL("Player needs to specify at least one letter that'll be exchanged!");
		return false;
	}

	if(ps.exchanges >= EXCHANGES_ALLOWED)
	{
		LOGL("Player already used all his exchanges!");
		return false;
	}

	//TODO sprawdzic czy to dziala? 
	auto letters = action.letters; //kopia pomocnicza
	FOREACH(char c, unique(sort(letters)))
	{
		//jesli chcemy wymienic wiecej liter ze znakiem c niz mamy, nie wolno
		const int ownedCount = count(ps.letters, c), putCount = count(action.letters, c);
		if(putCount > ownedCount) 
		{
			LOGFL("Player attempts to exchange %d letters '%c' but has only %d!", putCount % c % ownedCount);
			return false;
		}
	}

	return true;
}

int CGameState::pointsForWord(WordRange word) const
{
	PointsForWordsAccumulator pointsAccumulator;	
	//foreach letter in range
	for(Pos pos = word.first; ;pos += forwardDirection(word.getOrientation()))
	{
		const CTile &t = board.tile(pos);
		auto itrLetterToPoints = rules.pointsForLetter.find(*t.letter);
		if(itrLetterToPoints == rules.pointsForLetter.end())
		{
			LOGFL("Surprising failure: cannot find points for the letter '%c'", *t.letter);
			throw runtime_error("No such letter");
		}

		const int basicPoints = itrLetterToPoints->second; //no need to check for blank, addLetter does it
		pointsAccumulator.addLetter(t, basicPoints);

		if(pos == word.last)
			break;
	}

	return pointsAccumulator.totalPoints();
}

int CGameState::whoHasMostPoints() const
{
	auto playersInGame = players;
	erase_if(playersInGame, [](const CPlayerState &ps) { return ps.disqualified; });
	if(playersInGame.empty())
	{
		return -1;
	}

	return maxElementByFun(playersInGame, [](const CPlayerState &p) { return p.points; })->ID;
}

void CGameState::handleEndingCondition()
{
	//Check for ending conditions
	bool someoneHadntPassedTwice = false;
	bool someoneNotDisqualified = false;

	FOREACH(auto &ps, players)
	{
		if(!ps.disqualified)
			someoneNotDisqualified = true;
		else
			continue;

		//gra jest skonczona, jesli ktos zuzyl wszystkie litery, a worek jest pusty
		if(ps.letters.empty() && letters.empty()) 
		{
			finishGame(whoHasMostPoints(), str(format("Game is finished because player %d used all letters and the bag is empty.") % ps.ID));
			return;
		}
		if(ps.turnsSkipped < 2)
			someoneHadntPassedTwice = true;
	}

	//wszyscy spasowali przynajmniej dwa razy pod rzad, gra skonczona
	if(!someoneHadntPassedTwice)
	{
		finishGame(whoHasMostPoints(), "Game is finished because all players passed two times in a row.");
		return;
	}
	if(!someoneNotDisqualified)
	{
		finishGame(-1, "All players disqualified!");
		return;
	}
}

void CGameState::finishGame(int winner, string comment)
{
	LOGFL("Finishing game. The winner is player %d. Ending reson: %s", winner % comment);
	result = GameResult();
	result->victor = winner;
	result->comment = comment;

	for(int i = 0; i < players.size(); i++)
		if(players[i].disqualified)
			comment += str(format("\tplayer %d was disqualified because %s") % i % players[i].disqualificationReason);

}

void CPlayerState::writeData(boost::asio::ip::tcp::socket& sock) const
{
	uint16_t temp = htons(exchanges);
	write(sock, boost::asio::buffer(&temp, 2));
	temp = htons(turnsSkipped);
	write(sock, boost::asio::buffer(&temp, 2));
	temp = htons(ID);
	write(sock, boost::asio::buffer(&temp, 2));
	temp = htons(points);
	write(sock, boost::asio::buffer(&temp, 2));
	temp = htons(letters.size());
	write(sock, boost::asio::buffer(&temp, 2));
	FOREACH(auto &let, letters) {
	    write(sock, boost::asio::buffer(&let, 1));
	}
}

int CRules::letterValue(char c) const
{
	auto i = pointsForLetter.find(c);
	if(i != pointsForLetter.end())
		return i->second;

	return 0;

}

int CRules::wordBasicValue(crstring word) const
{
	int ret = 0;
	FOREACH(char c, word)
		ret += letterValue(c);

	return ret;
}

void CRules::writeData(boost::asio::ip::tcp::socket& sock) const
{
	uint16_t size = htons(pointsForLetter.size());
	write(sock, boost::asio::buffer(&size, 2));
	
	map<char, int>::const_iterator cit;
	char ch;
	uint16_t val;
	for(cit = pointsForLetter.begin(); cit != pointsForLetter.end(); ++cit) 
	{
		ch = (*cit).first;
		val = htons((*cit).second);
		write(sock, boost::asio::buffer(&ch, 1));
		write(sock, boost::asio::buffer(&val, 2));
	}
}