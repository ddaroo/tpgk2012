using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using System.Threading;
using System.IO;
using System.Text.RegularExpressions;

namespace Odpalarka
{
	static class Paths
	{
		public static string serverDirectory = @"T:\Scrabble_github\Serwer\";
		public static string serverExe = @"T:\Scrabble_github\Serwer\Serwer.exe";
		public static string serverDictioanry = @"T:\Scrabble_github\Serwer\dict.txt";
		public static string playersDirectory = @"T:\gracze";
		public static string javaExe = @"C:\Program Files (x86)\Java\jre7\bin\java.exe";

		public static string wyniki1 = @"T:\Scrabble_github\Serwer\WynikiFaza1.txt";
		public static string wyniki2 = @"T:\Scrabble_github\Serwer\WynikiFaza2.txt";

		public static Dictionary<string, string> jarToPath = new Dictionary<string, string>();
	}

	static class CombinationExt
	{
		//za http://stackoverflow.com/a/1898744
		public static IEnumerable<IEnumerable<T>> Combinations<T>(this IEnumerable<T> elements, int k)
		{
			return k == 0 ? new[] { new T[0] } :
			  elements.SelectMany((e, i) =>
				elements.Skip(i + 1).Combinations(k - 1).Select(c => (new[] { e }).Concat(c)));
		}
	}

	static class Helpers
	{
		public static Process startProcess(ProcessStartInfo psi)
		{
			System.Console.WriteLine("Odpalam " + psi.FileName + " z argumentami " + psi.Arguments);
			// 				psi.RedirectStandardError = true;
			// 				psi.RedirectStandardOutput = true;
			psi.UseShellExecute = false;
			return Process.Start(psi);
		}

		public static Process startServer(int port, string names, string arkuszWyn ="")
		{
			Trace.Assert(System.IO.File.Exists(Paths.serverExe));

			string serverArgs = @"-d" + Paths.serverDictioanry + " -c2 -p2 --names=" + names;
			if (arkuszWyn.Length > 0)
				serverArgs += " --results=" + arkuszWyn;

			ProcessStartInfo psi = new ProcessStartInfo(Paths.serverExe, serverArgs);
			psi.WorkingDirectory = Paths.serverDirectory;
			return Helpers.startProcess(psi);
		}

		public static void zaczekajAzSlucha()
		{
			System.Console.WriteLine("Czekam na serwer...");
			while (!File.Exists(Paths.serverDirectory + "listening"))
				Thread.Sleep(5);

			System.Console.WriteLine("Serwer slucha");
		}
	}

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{


		static string tournamentDir = @"T:\turniej\";
		static int defaultPort = 30750;

		struct Turniej
		{
			public PlayerInfo gracz;



			public void rozegraj()
			{
				Stopwatch sw = new Stopwatch();
				sw.Start();
				Process serv = Helpers.startServer(defaultPort, gracz.jarname);
				Helpers.zaczekajAzSlucha();

				Process play = gracz.startPlayer(defaultPort);
				serv.WaitForExit();
				//play.WaitForExit();

// 				System.Console.WriteLine(play.StandardOutput.ReadToEnd());
// 				System.Console.WriteLine(play.StandardError.ReadToEnd());
// 				System.Console.WriteLine(sw.Elapsed.ToString());
			}
		}

		struct Pojedynek
		{
			public List<PlayerInfo> players;


			public void rozegraj(string wyniki = "")
			{
				Stopwatch sw = new Stopwatch();
				sw.Start();

				string names = players[0].jarname;
				for (int i = 1; i < players.Count; i++)
					names += "," + players[1].jarname;

				Process serv = Helpers.startServer(defaultPort, names, wyniki);

				foreach (PlayerInfo gracz in players)
				{
					Helpers.zaczekajAzSlucha();
					Process play = gracz.startPlayer(defaultPort);
					Thread.Sleep(500);
				}

				serv.WaitForExit();

				//play.WaitForExit();

				// 				System.Console.WriteLine(play.StandardOutput.ReadToEnd());
				// 				System.Console.WriteLine(play.StandardError.ReadToEnd());
				// 				System.Console.WriteLine(sw.Elapsed.ToString());
			}
		}


		struct PlayerInfo
		{
			public string dir, jarname;

			public PlayerInfo(string Dir, string Jar)
			{
				dir = Dir;
				jarname = Jar;
			}

			public PlayerInfo(string pathToJar)
			{
				Trace.Assert(File.Exists(pathToJar));
				dir = System.IO.Path.GetDirectoryName(pathToJar);
				jarname = System.IO.Path.GetFileName(pathToJar);
			}

			public string fullpathToJar()
			{
				return dir + "\\" + jarname;
			}

			public Process startPlayer(int port)
			{
				string javaArgs = @"-Xmx50m -jar " + fullpathToJar() + " " + port.ToString() + " " + @"T:\dict.txt";

				ProcessStartInfo pgi = new ProcessStartInfo(Paths.javaExe, javaArgs);
				pgi.WorkingDirectory = dir;
				return Helpers.startProcess(pgi);
			}
		}

		public MainWindow()
		{
			InitializeComponent();
		}

		private void Grid_Drop_1(object sender, DragEventArgs e)
		{
			string[] droppedFilePaths = e.Data.GetData(DataFormats.FileDrop, true) as string[];
			foreach (string droppedFilePath in droppedFilePaths)
			{
			} 
		}

		void przerob()
		{
			Dictionary<string, List<int> > wyniki = new Dictionary<string, List<int> >();

			string arkusz = Paths.serverDirectory + "wielkiArkuszWynikow.txt";
			StreamReader sr = new StreamReader(arkusz);
			while (!sr.EndOfStream)
			{
				string s = sr.ReadLine();
				string[] elementy = s.Split('\t');
				if (elementy.Length != 7)
					continue;

				int suma = int.Parse(elementy[2]) + int.Parse(elementy[3]) + int.Parse(elementy[4]) + int.Parse(elementy[5]);
				int wynik = 100 * int.Parse(elementy[2]) / suma;
				if (wyniki.ContainsKey(elementy[0]))
					wyniki[elementy[0]].Add(wynik);
				else
					wyniki.Add(elementy[0], new List<int>{wynik});
			}
			sr.Close();

			StreamWriter moje = new StreamWriter(Paths.serverDirectory + "Wyniki.txt");
			foreach (var t in wyniki)
			{
				moje.Write(t.Key + '\t');
				moje.Write((int)t.Value.Average());
				moje.Write("pkt");
				foreach (var val in t.Value)
				{
					moje.Write('\t');
					moje.Write(val);
				}
				moje.WriteLine();
			}
			moje.Close();
		}

		List<PlayerInfo> scanForPlayers()
		{
			List<PlayerInfo> programy = new List<PlayerInfo>();
			foreach (string dir in Directory.EnumerateDirectories(Paths.playersDirectory))
				foreach (string file in Directory.EnumerateFiles(dir))
					if (file.EndsWith("jar"))
						programy.Add(new PlayerInfo(file));

			foreach (PlayerInfo p in programy)
			{
				Paths.jarToPath.Add(p.jarname, p.fullpathToJar());
			}

			return programy;
		}

		class StatystykiGracza
		{
			public int ileZwyciestw;
			public int ileMeczy;
			public int punktyRazem;
			public string nazwa;
		}

		Dictionary<string, StatystykiGracza> zbierzWyniki(string fname)
		{
			Dictionary<string, StatystykiGracza> wyniki = new Dictionary<string, StatystykiGracza>();
			
			StreamReader sr = new StreamReader(fname);
			while (!sr.EndOfStream)
			{
				string line = sr.ReadLine();
				string[] pola = line.Split('\t');

				for (int i = 0; i < 2; i++)
				{
					string name = pola[3+i];
					StatystykiGracza stat;
					if (wyniki.ContainsKey(name))
						stat = wyniki[name];
					else
					{
						stat = new StatystykiGracza();
						stat.nazwa = name;
						wyniki.Add(name, stat);
					}

					if (i == int.Parse(pola[9]))
						stat.ileZwyciestw++;

					stat.ileMeczy++;
					stat.punktyRazem += int.Parse(pola[6 + i]);
				}
			}
			sr.Close();

			return wyniki;
		}

		void rozegrajFazeTurnieju(List<PlayerInfo> gracze, string arkuszWynikow)
		{
			var pary = gracze.Combinations(2);

			List<Pojedynek> pojedynki = new List<Pojedynek>();

			foreach (var para in pary)
			{
				Pojedynek p = new Pojedynek();
				p.players = new List<PlayerInfo>();

				foreach (var gracz in para)
				{
					p.players.Add(gracz);
					System.Console.Write(gracz.jarname + "\t");
				}

				pojedynki.Add(p);
				System.Console.WriteLine();
			}

			foreach (Pojedynek p in pojedynki)
			{
				p.rozegraj(arkuszWynikow);
				p.players.Reverse();
				p.rozegraj(arkuszWynikow);
			}
		}

		List<StatystykiGracza> zrobRanking(Dictionary<string, StatystykiGracza> wyniki)
		{
			List<StatystykiGracza> ranking = new List<StatystykiGracza>(wyniki.Values);
			ranking.Sort((a, b) => a.ileZwyciestw.CompareTo(b.ileZwyciestw));
			ranking.Reverse();
			return ranking;
		}

		void WypiszWyniki(List<StatystykiGracza> ranking, string plik)
		{
			StreamWriter w = new StreamWriter(plik);

			foreach (StatystykiGracza g in ranking)
			{
				w.WriteLine(g.nazwa + "\t" + g.ileZwyciestw + "\t" + g.punktyRazem);
			}

			w.Close();
		}

		void oficjalnyTurniej()
		{
			rozegrajFazeTurnieju(scanForPlayers(), Paths.wyniki1);
			Dictionary<string, StatystykiGracza> wyniki = zbierzWyniki(Paths.wyniki1);
			List<StatystykiGracza> ranking = zrobRanking(wyniki);
			WypiszWyniki(ranking, Paths.serverDirectory + "WynikiPierwszejFazy.txt");

			List<PlayerInfo> drugaFaza = new List<PlayerInfo>();
			for (int i = 0; i < Math.Min(18, ranking.Count); i++)
				drugaFaza.Add(new PlayerInfo(Paths.jarToPath[ranking[i].nazwa]));

			rozegrajFazeTurnieju(drugaFaza, Paths.wyniki2);
			WypiszWyniki(zrobRanking(zbierzWyniki(Paths.wyniki2)), Paths.serverDirectory + "WynikiDrugiejFazy.txt");
		}

		private void Window_Loaded_1(object sender, RoutedEventArgs e)
		{
			//zbierzWyniki(Paths.serverDirectory + "wielkiArkuszWynikow.txt");
			oficjalnyTurniej();
			return;
 			//przerob();
 			//return;

			List<string> programy = new List<string>();
			foreach(string dir in Directory.EnumerateDirectories(Paths.playersDirectory))
			{
				foreach (string file in Directory.EnumerateFiles(dir))
				{
					if(file.EndsWith("jar"))
						programy.Add(file);

				}
			}

			foreach (string gracz in programy)
			{
// 				if (!gracz.Contains("Malkiewicz"))
// 					continue;

				for(int i = 0; i < 10; i++)
				{
					Turniej t;
					t.gracz = new PlayerInfo(gracz);
					t.rozegraj();
				}
			}

			Close();
		}
	}
}
