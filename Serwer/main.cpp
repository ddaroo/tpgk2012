// Scrabble.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Serwer.h"
#include "StopWatch.h"
#include "Dictionary.h"

po::variables_map vm; //tu trzymamy parametry przekazane w linii komend
vector<string> playerNames;

Logger logger;

const std::string DEF_RECORD = "./DD_MM_RR_HH_MM_SS_PLAYERS.txt";

void handleStartingParameters(int argc, char *argv[], po::variables_map &vm)
{
	CHECK_TIME("Parsing starting parameters");
	po::options_description opts("Allowed options");
	opts.add_options()
		("names", po::value<std::string>(), "comma separated names of player (0 or more) N0,N1,...")
		("seed,s", po::value<unsigned>()->default_value(0), "seed for random number generator")
		("help,h", "display help and exit")
		("dictionary,d",po::value<std::string>()->default_value("./dict.txt"), "file with dictionary")
		("players,p",po::value<unsigned>()->default_value(4), "number of participating players")
		("connections,c",po::value<unsigned>()->default_value(1), "number of socket-connected players to be expected")
		("moveTime", po::value<unsigned>()->default_value(15000), "time in milliseconds for player to make his move; 0 disables checking")
		("initTime", po::value<unsigned>()->default_value(30000), "time in milliseconds for player to initialize; 0 disables checking")
		("record,r", po::value<std::string>()->default_value(DEF_RECORD), "file with logs.")
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
	if(vm.count("names"))
	{
		boost::split(playerNames, vm["names"].as<string>(), boost::is_any_of(","));
	}

	string playerNamesForLog = vm["names"].as<string>();

	for(int i = 0; i < vm["players"].as<unsigned>(); i++)
	{
		string no = lexical_cast<string>(i);
		if(i >= playerNames.size())
			playerNames.push_back("Player nr " + no);
		else
			playerNames[i] += " (player " + no + ")";
	}

	string logname = vm["record"].as<string>();
	if(logname == DEF_RECORD)
	{
		time_t epoch_time;
		struct tm *tm_p;
		epoch_time = time( NULL );
		tm_p = localtime( &epoch_time );

		string logdir = str(format("./logs_%s/") % playerNamesForLog);
		boost::filesystem::create_directory(logdir);
		logname = logdir + str(format("%02d.%02d.%02d_%02d-%02d-%02d.log") % tm_p->tm_mday % tm_p->tm_mon % (tm_p->tm_year - 100) % tm_p->tm_hour % tm_p->tm_min % tm_p->tm_sec);
		//logname = "./testtt";
	}

	cout << "Logging to " << logname << std::endl;
	logger.fname = logname;
	logger.oficjalnyLog = make_unique<ofstream>(logname);
	if(!logger.oficjalnyLog || !*logger.oficjalnyLog)
	{
		cout << "Failed to open logfile!\n";
		logger.oficjalnyLog.release();
	}



	int seed = vm["seed"].as<unsigned>();
	if(!seed)
		seed = time(nullptr);

	logger << "Using random seed " << seed << std::endl;
	srand(seed);
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
		CStopWatch csw;
		s.run();

		if(vm.count("name0"))
		{
			ofstream wyniki("wielkiArkuszWynikow.txt", ios::app);
			wyniki << vm["name0"].as<string>() << "\t" << logger.fname << "\t";
			for(int i = 0; i < s.gs.players.size(); i++)
				wyniki << s.gs.players[i].points << "\t";
			wyniki << csw.getDiff() << endl;
		}
	} CATCH_LOG


	cout << flush;
	return EXIT_SUCCESS;
}