#pragma once



/*
 * timeHandler.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

class CStopWatch
{
	si64 start, last, mem;

public:
	CStopWatch();

	si64 getDiff(); //get diff in milliseconds
	void update();
	void remember();
	si64 memDif();

private:
	si64 clock();
};

struct TimeChecker
{
	CStopWatch timer;
	string text;

	TimeChecker(crstring _text);
	~TimeChecker();
};

#define CHECK_TIME(txt) TimeChecker hlp(txt);
#define CHECK_TIME_FORMAT(txt, args) TimeChecker hlp(boost::str(boost::format(txt) % args));

