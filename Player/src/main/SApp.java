package main;

import java.io.*;
import java.net.*;
import java.util.logging.*;

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
			
			SApp.mout = new DataOutputStream(SApp.msock.getOutputStream());
			SApp.min = new DataInputStream(SApp.msock.getInputStream());
		} catch (Exception e) {
			SApp.mlog.log(Level.SEVERE, e.getMessage());
			System.exit(ExitStat.FAIL_CON.ordinal());
		}
		
		try {
			// pobierz wiadomość powitalną z serwera
			int msgLen = SApp.min.readByte();
			StringBuilder sb = new StringBuilder();
			for(int i = 0; i < msgLen; ++i) {
				sb.append((char) SApp.min.readByte());
			}
			SApp.mlog.log(Level.INFO, "Wiadomość od serwera: ".concat(sb.toString()));
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
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
		
		SApp.initConnection(args[0]);
		// TODO implementation
	}
}
