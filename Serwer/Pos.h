#pragma once


struct Pos
{
	int x, y;


	Pos();
	Pos(int _x, int _y);
	Pos operator+(const Pos &rhs) const;
	Pos operator-() const;
	Pos & operator+=(const Pos &rhs);
	bool operator<(const Pos &rhs) const;
	bool operator!=(const Pos &rhs) const;
	bool operator==(const Pos &rhs) const;

	bool isValid() const;
	vector<Pos> validNeighbors() const;

	string toString() const;
};


ostream & operator<<(ostream &out, const Pos &p);

char intToChar(int i); //pomocnicza funkcja do oznaczania numerow wierszy/kolumn w pseudo-wizualizacji

enum EOrientation {HORIZONTAL = 0, VERTICAL = 1, INVALID};

EOrientation flip(EOrientation or);
EOrientation orientationOfPos(const Pos &a, const Pos &b); //sprawdza, czy litery leza na prostej poziomej czy pionowej

static const EOrientation orientations[] = {HORIZONTAL, VERTICAL};
static const Pos RIGHT = Pos(1, 0), LEFT(-1, 0), UP(0,-1), DOWN(0,1);
static const Pos directions[] = {RIGHT, DOWN, LEFT, UP};

Pos forwardDirection(EOrientation orientation);
Pos backwardDirection(EOrientation orientation);

struct WordRange
{
	Pos first, last;
	WordRange(Pos f, Pos l);
	EOrientation getOrientation() const;
	int length() const;
};
