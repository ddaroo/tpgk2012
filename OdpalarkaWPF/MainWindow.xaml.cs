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

namespace Odpalarka
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		static string serverDirectory = @"T:\Scrabble_github\Serwer\";
		static string serverExe = @"T:\Scrabble_github\Serwer\Serwer.exe";
		static string serverDictioanry = @"T:\Scrabble_github\Serwer\dict.txt";
		static string playersDirectory = @"T:\gracze";
		static string javaExe = @"C:\Program Files (x86)\Java\jre7\bin\java.exe";


		static string tournamentDir = @"T:\turniej\";
		static int defaultPort = 30750;

		struct Turniej
		{
			public PlayerInfo gracz;

			Process startProcess(ProcessStartInfo psi)
			{
				System.Console.WriteLine("Odpalam " + psi.FileName + " z argumentami " + psi.Arguments);
// 				psi.RedirectStandardError = true;
// 				psi.RedirectStandardOutput = true;
 				psi.UseShellExecute = false;
				return Process.Start(psi);
			}

			Process startPlayer(PlayerInfo pi, int port)
			{
				string javaArgs = @"-Xmx50m -jar " + pi.fullpathToJar() + " " + port.ToString() + " " + @"T:\dict.txt";

				ProcessStartInfo pgi = new ProcessStartInfo(javaExe, javaArgs);
				pgi.WorkingDirectory = pi.dir;
				return startProcess(pgi);
			}

			Process startServer(int port)
			{
				Trace.Assert(System.IO.File.Exists(serverExe));

				string serverArgs = @"-d" + serverDictioanry + " --names=" + gracz.jarname;

				ProcessStartInfo psi = new ProcessStartInfo(serverExe, serverArgs);
				psi.WorkingDirectory = serverDirectory;
				return startProcess(psi);
			}

			void zaczekajAzSlucha()
			{
				System.Console.WriteLine("Czekam na serwer...");
				while (!File.Exists(serverDirectory + "listening"))
					Thread.Sleep(5);

				System.Console.WriteLine("Serwer slucha");
			}

			public void rozegraj()
			{
				Stopwatch sw = new Stopwatch();
				sw.Start();
				Process serv = startServer(defaultPort);
				zaczekajAzSlucha();

				Process play = startPlayer(gracz, defaultPort);
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

			string arkusz = serverDirectory + "wielkiArkuszWynikow.txt";
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

			StreamWriter moje = new StreamWriter(serverDirectory + "Wyniki.txt");
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

		private void Window_Loaded_1(object sender, RoutedEventArgs e)
		{
 			//przerob();
 			//return;

			List<string> programy = new List<string>();
			foreach(string dir in Directory.EnumerateDirectories(playersDirectory))
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
