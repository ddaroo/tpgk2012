#include "stdafx.h"
#include "PlayerDrivers.h"
#include "Serwer.h"
#include "Dictionary.h"

TAction CppDummyPlayer::takeAction(const CBoard &b, const CPlayerState &ps, const CRules &r)
{
	auto printLetters = [](vector<char> &l, int c)
	{
		std::cout << "[ " << l[0];
		for (int j = 1; j < c; ++j) 
			std::cout << l[j];
		std::cout << " ]" << std::endl;
	};

	vector<pair<CLetter, Pos> > lettersOnBoard;
	b.foreachTile([&](const CTile &t)
	{
		if(t.letter)
			lettersOnBoard.push_back(make_pair(*t.letter, t.pos));
	});

	if(lettersOnBoard.empty())
	{
		//na starcie trzeba ulozyc slowo idace przez srodek z 2+ liter
		vector<string> possibleWords;
		vector<char> letters = ps.letters;

		sort(letters);

		LOGL("Using brute-force... Mine letters:");
		printLetters(letters, letters.size());

		int N = 0;

		for(int i = letters.size()-1; i >= 0; i--)
		{
			int r = i+1;
			CHECK_TIME("Checking all words of length " + lexical_cast<string>(r));

			vector<char> letters2;
			letters2.resize(r+1, 0);

			do
			{
				copy(letters.begin(), letters.begin() + r, letters2.begin());
				N++;
				if(dictionary->contains(&letters2[0]))
					possibleWords.emplace_back(&letters2[0]);
			} while(boost::next_partial_permutation(letters.begin(), letters.begin() + r, letters.end()));
		}

		LOGFL("After testing %d permutations, found %d suitable words:", N % possibleWords.size());
		for (int i = 0; i < (int)possibleWords.size(); i++)
			LOGFL("\t%d. %s", i % possibleWords[i]);

		if(possibleWords.empty())
		{
			//TODO exchange some letters
			return SkipTurn();
		}



		sort(possibleWords, [&](crstring lhs, crstring rhs)
		{
			return r.wordBasicValue(lhs) < r.wordBasicValue(rhs);
		});

		string chosen = possibleWords.back();
		LOGFL("The best word is %s worth %d points.", chosen % r.wordBasicValue(chosen));
		PutLetters ret;
		Pos curPos = Pos(WIDTH / 2, HEIGHT / 2); //start with center tile
		Pos direction = Pos(0,1); //downwards
		ret.orientation = VERTICAL;
		FOREACH(char c, chosen)
		{
			PutLetters::PutLetter pl;
			pl.letter = c;
			pl.pos = curPos;
			ret.letters.push_back(pl);

			curPos = curPos + direction;
		}

		return ret;
	}
	else
	{
		//robimy zbior pol z przyleglymi literami -> tam mozna dokladac
		set<Pos> neighbours;
		FOREACH(auto &letter, lettersOnBoard)
			FOREACH(auto &neighPos, letter.second.validNeighbors())
				if(!b.tile(neighPos).letter)
					neighbours.insert(neighPos);


		vector<pair<PutLetters::PutLetter, int> > possibilities; //pairs[put letter, points gained]

		FOREACH(const auto &pos, neighbours)
		{
			FOREACH(char l, ps.letters)
			{
				auto possibleOutcomes = b.wordsGeneratedbyLetter(l, pos);
				if(possibleOutcomes.size())
				{
					bool bad = false;
					int value = 0;
					FOREACH(crstring word, possibleOutcomes)
					{
						if(dictionary->contains(word.c_str()))
							value += r.wordBasicValue(word);
						else
							bad = true;
					}

					if(value && !bad)
					{
						PutLetters::PutLetter pl;
						pl.letter = l;
						pl.pos = pos;
						possibilities.push_back(make_pair(pl, value));
					}
				}
			}
		}

		sort(possibilities, [](const pair<PutLetters::PutLetter, int> &lhs, const pair<PutLetters::PutLetter, int> &rhs)
		{
			return lhs.second < rhs.second;
		});

		if(possibilities.size())
		{
			PutLetters ret;
			ret.letters.push_back(possibilities.back().first);
			if(b.wordGeneratedbyLetter(possibilities.back().first.letter, possibilities.back().first.pos, HORIZONTAL).size() > 1)
				ret.orientation = HORIZONTAL;
			else 
				ret.orientation = VERTICAL;

			return ret;
		}
		else if(ps.exchanges < EXCHANGES_ALLOWED)
		{
			ExchangeLetters el;
			el.letters = ps.letters;
			return el;
		}
	}

	return SkipTurn();
}

void CppDummyPlayer::init()
{

}

JavaPlayer::JavaPlayer(string programName)
	: socket(io)
{
	unsigned short portNumber = 30750;

	string startingCommand = str(format("java -jar %s") % programName);
	LOGFL("The player program will be called by a command \"%d\"", startingCommand);
	systemCallThread = make_unique<boost::thread>([=] { system(programName.c_str()); }); //odpalamy wywolanie systemowe na nowym watku

	try
	{		
		TAcceptor acceptor(io, TEndpoint(boost::asio::ip::tcp::v4(), portNumber)); //przyjmuje polaczenia

		LOGFL("Listening for TCP connection at port %d...", portNumber);
		acceptor.accept(socket); //przyjecie polaczenia do socketu
		LOGFL("TCP connection at port %d has been successfully established!", portNumber);
		
		int written = write(socket, boost::asio::buffer("Hello!"));


	} CATCH_LOG

}

TAction JavaPlayer::takeAction(const CBoard &b, const CPlayerState &ps, const CRules &r)
{
	//TODO wyszarpac z socketu potrzebne dane

	return SkipTurn(); //lub inna, bardziej stosowna akcja
}

void JavaPlayer::init()
{
	//TODO przekazac do socketu niezmienniki
}

JavaPlayer::~JavaPlayer()
{

}