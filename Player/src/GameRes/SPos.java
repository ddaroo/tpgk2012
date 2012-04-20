package GameRes;

import java.io.*;

/**
 * Pozycja płytki na planszy.
 *
 */
public class SPos implements Serializable {
	public int mx;
	public int my;
	
	public SPos() {}

	private void readObject(ObjectInputStream sin) throws IOException,
			ClassNotFoundException {
		mx = sin.readInt();
		my = sin.readInt();
	}
}
