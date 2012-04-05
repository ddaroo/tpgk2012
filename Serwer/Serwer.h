#pragma once
#include "Pos.h"
#include "Actions.h"

struct IPlayerDriver;

struct CTile
{
	Pos pos;
	boost::optional<CLetter> letter;
	bool letterGeneratedFromBlank;

	struct Bonus
	{
		enum EBonusType {NONE, LETTER, WORD} multiplicatorType; //czy mnoznik jest do litery czy do calego slowa
		int multiplicatorValue; //ilokrotny mnoznik

		Bonus(EBonusType type = NONE, int value = 0)
		{
			multiplicatorType = type;
			multiplicatorValue = value;
		}
	} bonus;

	CTile()
	{
		letterGeneratedFromBlank = false;
	}
};

//plansza
struct CBoard : boost::noncopyable 
{
	vector<vector<CTile> > tiles; //wiem, ze mozna multi_array, ale tak lepiej wyglada w debugerze

	CBoard();

	CTile &tile(Pos p);
	const CTile &tile(Pos p) const;

	string lettersNextTo(Pos pos, EOrientation orientation, bool forward) const; //DOESNT include the pos itself
	string wordGeneratedbyLetter(char letter, Pos pos, EOrientation orientation) const; //if the letter were to be put on tile, what word would be created horizontally/vertically
	vector<string> wordsGeneratedbyLetter(char letter, Pos pos) const; //if the letter were to be put on tile, what words will be created
	Pos lastLetterInDirection(Pos src, Pos dir) const; //idzie w kierunku dir, dopoki sa litery; zwraca pozycje skrajnej
	string readWord(Pos from, Pos dir) const; //reads from given tile the word, until an empty tile is ecnountered

	WordRange getWordRange(Pos pos, EOrientation orientation);

	//algorithms
	void foreachPos(function<void(Pos)> func) const; //calls func with every valid (x,y)
	void foreachTile(function<void(const CTile &)> func) const; //calls func with every valid tile -> will go in rows from top
};

struct CPlayerState
{
	int exchanges; //ile wymian dokonal
	int turnsSkipped;
	int ID;
	int points; //liczba zdobytych do tej pory punktow
	vector<CLetter> letters; //stojak z literkami

	CPlayerState(int _ID)
	{
		exchanges = 0;
		turnsSkipped = 0;
		ID = _ID;
		points = 0;
	}
};


struct CRules
{
	map<char, int> pointsForLetter;

	int letterValue(char c) const;
	int wordBasicValue(crstring word) const;
};

struct CGameState : boost::noncopyable
{
	int turn; //from 0, -1 before game start
	unsigned activePlayer; //pozycja gracza w wektorze
	CBoard board;
	vector<CLetter> letters; //worek z literkami
	vector<CPlayerState> players;
	CRules rules;

	void createLetters();
	void addNewPlayer();

	void startGame();
	int nextMove(); //przygotowuje stan do ruchu nastepnego gracza, zwraca ID tego, kogo nalezy zapytac o ruch
	void applyAction(const TAction &action);
	void drawLetters(CPlayerState &ps, unsigned howMany);

	void applyAction(const PutLetters &action);
	void applyAction(const SkipTurn &action);
	void applyAction(const ExchangeLetters &action);

	//functions validating action requests -> return true if possible
	bool validateAction(const PutLetters &action, const CPlayerState &ps) const;
	bool validateAction(const SkipTurn &action, const CPlayerState &ps) const;
	bool validateAction(const ExchangeLetters &action, const CPlayerState &ps) const;

	void print() const;
	int pointsForWord(WordRange word) const;

	bool isGameFinished() const;

	CGameState();
};


class CSerwer : boost::noncopyable
{
public:
	CGameState gs;
	map<int, IPlayerDriver*> playerDrivers;

	CSerwer(void);
	~CSerwer(void);

	void run();
};


// struct PlayerViaSocker : IPlayerDriver
// {
// 
// };

