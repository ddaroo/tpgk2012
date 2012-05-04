package gameres;

import java.io.*;

/**
 * Pozycja płytki na planszy.
 */
public class SPos {
	public SPos() {
		mx = -1;
		my = -1;
	}
	
	public SPos(short x, short y) {
		setCoords(x, y);
	}
	
	public short x() {
		return mx;
	}
	
	public short y() {
		return my;
	}
	
	public void setCoords(short x, short y) {
		mx = x;
		my = y;
	}
	
	public void setX(short x) {
		mx = x;
	}
	
	public void setY(short y) {
		my = y;
	}
	
	public void writeData(DataOutputStream dout) throws IOException {
		dout.writeShort(my);
		dout.writeShort(mx);
	}

	public void readData(DataInputStream din) throws IOException {
		my = din.readShort();
		mx = din.readShort();
	}
	
	private short mx;
	private short my;
}
