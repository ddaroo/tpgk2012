package main;

import java.io.*;

public class SMsg {
	public SMsg() {}
	
	public SMsg(String msg) {
		mstr = msg;
	}
	
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
	
	public void writeData(DataOutputStream dout) throws IOException {
		dout.writeByte(mstr.length());
		for(short i = 0; i < mstr.length(); ++i) {
			dout.writeByte(mstr.charAt(i));
		}
	}
	
	private String mstr;
}
