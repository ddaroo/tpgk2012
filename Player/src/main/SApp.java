package main;

import java.io.*;
import java.net.*;
import java.util.logging.*;
import Player.*;

public class SApp {	
	public static Socket msock;
	public static DataOutputStream mout;
	public static DataInputStream min;
	public static Logger mlog = Logger.getLogger("Slog");
	
	public enum ExitStat { ALL_OK, FAIL_CON };
	
	public static void initConnection(String port) {
		try {
			SApp.msock = new Socket();
			InetSocketAddress addr = new InetSocketAddress(
					InetAddress.getByName("localhost"), Integer.parseInt(port));

			String logmsg = new String().concat("Łączenie z serwerem gry ")
					.concat(addr.getHostName()).concat(" używając portu ")
					.concat(String.valueOf((addr.getPort())));

			SApp.mlog.log(Level.INFO, logmsg);
			SApp.msock.connect(addr); // połącz się z serwerem gry
			
			SApp.mout = new DataOutputStream(
							new BufferedOutputStream(
									SApp.msock.getOutputStream()));
			SApp.min = new DataInputStream(
							new BufferedInputStream(
									SApp.msock.getInputStream()));
		} catch (Exception e) {
			SApp.mlog.log(Level.SEVERE, e.getMessage());
			System.exit(ExitStat.FAIL_CON.ordinal());
		}
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			// logowanie komunikatów do pliku
			FileHandler fh = new FileHandler("ScrabPlayer.log");
			fh.setFormatter(new SimpleFormatter());
			mlog.addHandler(fh);
			mlog.setLevel(Level.ALL);
		} catch(Exception e) {
			e.printStackTrace();
		}
		
		if(args.length == 0) {
			SApp.mlog.log(Level.INFO, "Usage: program <port>");
			System.exit(ExitStat.ALL_OK.ordinal());
		}
		
		SApp.initConnection(args[0]); // inicjuj połączenie z serwerem gry
		
		try {
			// pobierz wiadomość powitalną z serwera
			SMsg msg = new SMsg();
			msg.readData(SApp.min);
			SApp.mlog.log(Level.INFO, "Wiadomość od serwera: ".concat(msg.content()));
		} catch(Exception e) {
			e.printStackTrace();
		}
		
		SPlayer player = new SPlayer(min, mout);
		player.playGame(); // główna pętla programu
	}
}
