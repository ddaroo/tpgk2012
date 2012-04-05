#pragma once

#include "Pos.h"

struct ExchangeLetters
{
	vector<CLetter> letters;
};

struct PutLetters
{
	struct PutLetter
	{
		CLetter letter;
		Pos pos;
		bool generateFromBlank; //czy ta litera ma zostac wygenerowana z posiadanej blankowej plytki

		PutLetter()
		{
			letter = 0;
			generateFromBlank = false;
		}
	};

	vector<PutLetter> letters; //letters that we place
	EOrientation orientation; //HORIZONTAL or VERTICAL

	PutLetters()
	{
		orientation = INVALID;
	}
};

struct SkipTurn
{
};

typedef boost::variant<ExchangeLetters, PutLetters, SkipTurn> TAction;