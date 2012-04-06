// Scrabble.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Serwer.h"
#include "StopWatch.h"
#include "Dictionary.h"

namespace po = boost::program_options;

void handleStartingParameters(int argc, char *argv[], po::variables_map &vm)
{
	CHECK_TIME("Parsing starting parameters");
	po::options_description opts("Allowed options");
	opts.add_options()
		("seed,s", po::value<unsigned>()->default_value(0), "seed for random number generator")
		("help,h", "display help and exit")
		("dictionary,d",po::value<std::string>()->default_value("./dict.txt"), "file with dictionary");
	po::store(po::parse_command_line(argc, argv, opts), vm);
	po::notify(vm);

	if(vm.count("help"))
	{
		//print info and quit
		LOGL(NAME);
		LOGL(opts);
		exit(EXIT_SUCCESS);
	}

	//inicjalizacja RNG -> powtarzalnosc jest uzyteczna w testach
	srand(vm["seed"].as<unsigned>());
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
	system("chcp 1250"); //zeby polskie znaki sie wyswietlaly (UWAGA - nie dziala z rastrowymi czcionkami w cmd.exe)
#endif
	cout << "Polskie znaki:\n\tzazolc gesla jazn\n\tzażółć gęślą jaźń\n";
	try
	{
		po::variables_map vm; //tu trzymamy parametry przekazane w linii komend
		handleStartingParameters(argc, argv, vm);

		CDictionary dict(vm["dictionary"].as<string>());

		assert(dict.contains("hybryda"));
		assert(!dict.contains("hyrbyda"));
		assert(dict.contains("zażółć"));
		assert(!dict.contains("zażólć"));

		CSerwer s;
		s.run();

	} CATCH_LOG


	return EXIT_SUCCESS;
}