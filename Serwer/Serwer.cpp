#include "stdafx.h"
#include "Serwer.h"
#include "StopWatch.h"
#include "Dictionary.h"
#include "PlayerDrivers.h"

CSerwer::CSerwer(void)
{
	FOREACH(auto &ps, gs.players)
	{
		//TODO tutaj bedziemy tworzyc graczy socketowych, na razie dummy w c++

		playerDrivers[ps.ID] = new CppDummyPlayer();
		playerDrivers[ps.ID]->init();
	}
}

CSerwer::~CSerwer(void)
{
}

void CSerwer::run()
{
	do
	{
		int playerToMove = gs.nextMove();
		TAction action;
		{
			CHECK_TIME_FORMAT("Player %d was thinking", playerToMove);
			action = playerDrivers[playerToMove]->takeAction(gs.board, gs.players[playerToMove], gs.rules);
		}
		gs.applyAction(action);
	} while(!gs.isGameFinished());
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

void CBoard::foreachTile(function<void(const CTile &)> func) const
{
	for (int j = 0; j < HEIGHT ; j++)
		for (int i = 0; i < WIDTH ; i++)
			func(tiles[i][j]);
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

CGameState::CGameState()
{
	activePlayer = turn = -1;
	createLetters();

	for (int i = 0; i < PLAYER_COUNT; i++)
		addNewPlayer();
}

void CGameState::createLetters()
{
	ifstream lettersInfo("conf_letters.txt");
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
	bool someoneHadntPassedTwice = false;

	FOREACH(auto &ps, players)
	{
		//gra jest skonczona, jesli ktos zuzyl wszystkie litery, a worek jest pusty
		if(ps.letters.empty() && letters.empty()) 
		{
			LOGFL("Game is finished because player %d used all letters and the bag is empty.", ps.ID);
			return true; 
		}
		if(ps.turnsSkipped < 2)
			someoneHadntPassedTwice = true;
	}
	
	//wszyscy spasowali przynajmniej dwa razy pod rzad, gra skonczona
	if(!someoneHadntPassedTwice)
	{
		LOGL("Game is finished because all players passed two times in a row.");
		return true;  
	}

	return false;
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
	LOGFL("Applying action: Player %d exchanges %d letters:", ps.ID % action.letters.size() % formatLetters(action.letters));
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
	cout << "\t |";
	for (int i = 0; i < WIDTH ; i++)
		cout << intToChar(i);
	cout << "\n\t-+";
	for (int i = 0; i < WIDTH ; i++)
		cout << '-';
	cout << '\n';

	for (int j = 0; j < HEIGHT ; j++)
	{
		cout << "\t" << intToChar(j) << "|";
		for (int i = 0; i < WIDTH ; i++)
		{
			const auto &l = board.tiles[i][j].letter;
			if(l)
				cout << *l;
			else
				cout << '.';
		}
		cout << "\n";
	}

	LOGL("Players in game:");
	FOREACH(const auto &ps, players)
	{
		LOGF("\t%d - %d points, available letters: ", ps.ID % ps.points);
		FOREACH(char c, ps.letters)
			LOG(c);
		LOG("\n");
	}

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

	FOREACH(auto &ps, action.letters)
	{
		string perpendicularWord = board.wordGeneratedbyLetter(ps.letter, ps.pos, flip(action.orientation));
		if(perpendicularWord.size() > 1)
		{
			LOGF("Perpendicular word created: \"%s\". ", perpendicularWord);
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
		afterWord = board.lettersNextTo(action.letters.front().pos, action.orientation, true);

	const string wholeWord = beforeWord + wordCenter + afterWord;
	LOGF("Player attempts to create a word: \"%s\". ", wholeWord);
	if(dictionary->contains(wholeWord))
		LOGL("It's valid.");
	else
	{
		LOGL("It's invalid!");
		return false;
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