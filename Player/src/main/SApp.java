package main;

import java.io.*;
import java.net.*;
import java.util.logging.*;

public class SApp {	
	public static Socket msock;
	public static OutputStream mout;
	public static InputStream min;
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
			
			SApp.mout = SApp.msock.getOutputStream();
			SApp.min = SApp.msock.getInputStream();
		} catch (Exception e) {
			SApp.mlog.log(Level.SEVERE, e.getMessage());
			System.exit(ExitStat.FAIL_CON.ordinal());
		}
		
		try {
			// pobierz wiadomość powitalną z serwera
			int msgLen = SApp.min.read();
			byte[] msg = new byte[msgLen];
			int bytes = SApp.min.read(msg, 0, msgLen);
			if(msgLen != bytes) {
				String errmsg = new String("Przeczytano ")
									.concat(String.valueOf(bytes))
									.concat(" bajtów, spodziewano się ")
									.concat(String.valueOf(msgLen))
									.concat(" bajtów");
				throw new IOException(errmsg);
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if(args.length == 0) {
			SApp.mlog.log(Level.INFO, "It's working!");
			System.exit(ExitStat.ALL_OK.ordinal());
		}
		
		SApp.initConnection(args[0]);	
		// TODO implementation
	}
}
