// Scrabble.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Serwer.h"
#include "StopWatch.h"
#include "Dictionary.h"

po::variables_map vm; //tu trzymamy parametry przekazane w linii komend

void handleStartingParameters(int argc, char *argv[], po::variables_map &vm)
{
	CHECK_TIME("Parsing starting parameters");
	po::options_description opts("Allowed options");
	opts.add_options()
		("seed,s", po::value<unsigned>()->default_value(0), "seed for random number generator")
		("help,h", "display help and exit")
		("dictionary,d",po::value<std::string>()->default_value("./dict.txt"), "file with dictionary")
		("players,p",po::value<unsigned>()->default_value(4), "number of participating players")
		("connections,c",po::value<unsigned>()->default_value(1), "number of socket-connected players to be expected")
		("moveTime", po::value<unsigned>()->default_value(15000), "time in milliseconds for player to make his move; 0 disables checking")
		("initTime", po::value<unsigned>()->default_value(30000), "time in milliseconds for player to initialize; 0 disables checking")
		("lenient,l", "when player takes an illegal action, server complains but doesn't end the game");
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

	cout << "Program wypisuje znaki w kodowaniu windows-1250. Ponizej test polskich znakow:\n";
	const char zazolc1250[] = {'z', 'a', (char) 0xbf, (char) 0xf3, (char) 0xb3, (char) 0xe6, ' ', 'g', (char) 0xea, (char) 0x9c, 'l', (char) 0xb9,
								' ', 'j', 'a', (char) 0x9f, (char) 0xf1, (char) 0};
	const char *zazolcBezPol = "zazolc gesla jazn";

	cout << "\t" << zazolcBezPol << "\n\t" << zazolc1250 << endl;
	try
	{
		handleStartingParameters(argc, argv, vm);

		CDictionary dict(vm["dictionary"].as<string>());

// 		assert(dict.contains("hybryda"));
// 		assert(!dict.contains("hyrbyda"));
// 		assert(dict.contains("zażółć"));
// 		assert(!dict.contains("zażólć"));

		CSerwer s;
		s.run();
	} CATCH_LOG


	cout << flush;
	return EXIT_SUCCESS;
}