#pragma once

//zeby uciszyc troche warningow z bibliotek
#define _SCL_SECURE_NO_WARNINGS

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/multi_array.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "Combination.h"


#define FOREACH BOOST_FOREACH 

//to save typing
using std::string;
typedef const string & crstring;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;

using boost::lexical_cast;
using boost::str;
using boost::format;

using namespace std;
using namespace boost::range;
namespace po = boost::program_options;


const string NAME = "Scrabble Server v3";

#ifdef __FreeBSD__
	#include <sys/types.h>
	#include <sys/time.h>
	#include <sys/resource.h>
	#define TO_MS_DIVISOR (1000)
#else
	#define TO_MS_DIVISOR (CLOCKS_PER_SEC / 1000)
#endif

// Integral data types
typedef std::uint64_t ui64; //unsigned int 64 bits (8 bytes)
typedef std::uint32_t ui32;  //unsigned int 32 bits (4 bytes)
typedef std::uint16_t ui16; //unsigned int 16 bits (2 bytes)
typedef std::uint8_t ui8; //unsigned int 8 bits (1 byte)
typedef std::int64_t si64; //signed int 64 bits (8 bytes)
typedef std::int32_t si32; //signed int 32 bits (4 bytes)
typedef std::int16_t si16; //signed int 16 bits (2 bytes)
typedef std::int8_t si8; //signed int 8 bits (1 byte)

//nullptr -  only msvc and gcc-4.6 or later, othervice define it  as NULL
#if !defined(_MSC_VER) && !(defined(__GNUC__) && (GCC_VERSION >= 406))
	#define nullptr NULL
#endif

//override keyword - only msvc and gcc-4.7 or later.
#if !defined(_MSC_VER) && !(defined(__GNUC__) && (GCC_VERSION >= 407))
	#define override
#endif


//log text
#define LOG(text) std::cout << text

//log text and put new line
#define LOGL(text) do{LOG(text); LOG("\n"); }while(0)

//log text with format substitution and new line
#define LOGFL(text, operands) do {LOGL(boost::format(text) % operands);}while(0)

//log text with format substitution and new line
#define LOGF(text, operands) do {LOG(boost::format(text) % operands);}while(0)

//add catch blocks that log the exception
#define CATCH_LOG \
	catch(std::exception &e) \
	{ \
		LOGFL("An exception: %s", e.what()); \
	} \
		catch(...) \
	{ \
		LOGL("Unknown exception!"); \
	} 

#define MY_THROW(error_msg) do{LOGL(error_msg); throw std::runtime_error(error_msg);}while(0)

namespace vstd
{
	//returns true if container c contains item i
	template <typename Container, typename Item>
	bool contains(const Container & c, const Item &i)
	{
		auto b = std::begin(c),
			e = std::end(c);
		return std::find(b, e, i) != e;
	}

	//removes element i from container c, returns false if c does not contain i
	template <typename Container, typename Item>
	typename Container::size_type operator-=(Container &c, const Item &i)
	{
		typename Container::iterator itr = find(c,i);
		if(itr == c.end())
			return false;
		c.erase(itr);
		return true;
	}

	//assigns greater of (a, b) to a and returns maximum of (a, b)
	template <typename t1, typename t2>
	t1 &amax(t1 &a, const t2 &b)
	{
		if(a >= b)
			return a;
		else
		{
			a = b;
			return a;
		}
	}

	//assigns smaller of (a, b) to a and returns minimum of (a, b)
	template <typename t1, typename t2>
	t1 &amin(t1 &a, const t2 &b)
	{
		if(a <= b)
			return a;
		else
		{
			a = b;
			return a;
		}
	}

	//makes a to fit the range <b, c>
	template <typename t1, typename t2, typename t3>
	t1 &abetween(t1 &a, const t2 &b, const t3 &c)
	{
		amax(a,b);
		amin(a,c);
		return a;
	}

	//checks if low <= a <= high
	template <typename t1, typename t2, typename t3>
	bool iswithin(const t1 &a, const t2 &low, const t3 &high)
	{
		return a >= low && a <= high;
	}

	template<typename T>
	std::unique_ptr<T> make_unique()
	{
		return std::unique_ptr<T>(new T());
	}
	template<typename T, typename Arg1>
	std::unique_ptr<T> make_unique(Arg1&& arg1)
	{
		return std::unique_ptr<T>(new T(std::forward<Arg1>(arg1)));
	}

	//deletes pointer and sets it to NULL
	template <typename T>
	void clearPtr(T* &ptr)
	{
		delete ptr;
		ptr = NULL;
	}
}
using namespace vstd;


#include "StopWatch.h"


const int EXPECTED_WORD_COUNT = 3000000; //trzy miliony
const int LETTERS_RECEIVED_AT_START = 7;
const int PLAYER_COUNT = 4;
const int WIDTH = 15, HEIGHT = 15; //wymiary planszy
const int EXCHANGES_ALLOWED = 3; //trzy razy na gre mozna wymieniac litery

typedef char CLetter;
const CLetter BLANK = '_';
inline string formatLetters(const vector<CLetter> &letters)
{
	ostringstream out;
	if(letters.empty())
	{
		out << "No letters";
	}
	else
	{
		for(int i = 0; i < (int)letters.size() - 1;i++)
			out << letters[i] << ",";

		out << letters.back();
	}
	return out.str();
}


extern po::variables_map vm; //tu trzymamy parametry przekazane w linii komend