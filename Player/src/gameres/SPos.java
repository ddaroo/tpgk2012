package gameres;

import java.io.*;

/**
 * Pozycja płytki na planszy.
 */
public class SPos {
	public SPos() {}
	
	public short x() {
		return mx;
	}
	
	public short y() {
		return my;
	}
	
	public void setX(short x) {
		mx = x;
	}
	
	public void setY(short y) {
		my = y;
	}
	
	public void writeData(DataOutputStream dout) throws IOException {
		dout.writeShort(mx);
		dout.writeShort(my);
	}

	public void readData(DataInputStream din) throws IOException {
		mx = din.readShort();
		my = din.readShort();
	}
	
	private short mx;
	private short my;
}
