package main;

import java.io.*;

public class SMsg {
	public SMsg() {}
	
	/**
	 * @return Treść wiadomości
	 */
	public String content() {
		return mstr;
	}
	
	public void readData(DataInputStream din) throws IOException {
		short msgLen = SApp.min.readShort();
		StringBuilder sb = new StringBuilder();
		for(short i = 0; i < msgLen; ++i) {
			sb.append((char) din.readByte());
		}
		mstr = sb.toString();
	}
	
	private String mstr;
}
