#include "stdafx.h"
#include "StopWatch.h"


CStopWatch::CStopWatch() : start(clock())
{
	last=clock();
	mem=0;
}

si64 CStopWatch::getDiff() /*get diff in milliseconds */
{
	si64 ret = clock() - last;
	last = clock();
	return ret / TO_MS_DIVISOR;
}

void CStopWatch::update()
{
	last = clock();
}

void CStopWatch::remember()
{
	mem = clock();
}

si64 CStopWatch::memDif()
{
	return clock()-mem;
}

si64 CStopWatch::clock()
{
#ifdef __FreeBSD__
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	return static_cast<si64>(usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1000000 + usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
#else
	return std::clock();
#endif
}

TimeChecker::TimeChecker(crstring _text) : text(_text)
{

}

TimeChecker::~TimeChecker()
{
	LOGFL("%s: %d ms", text % timer.getDiff());
}