package GameRes;

import java.io.*;
import java.util.ArrayList;

/**
 * Plansza do gry Scrabble.
 *
 */
public class SBoard implements Serializable {
	/**
	 * Lista płytek na planszy.
	 */
	ArrayList<ArrayList<STile>> mb;
	// TODO implementation
	public SBoard() {}

	private void readObject(ObjectInputStream sin) throws IOException,
			ClassNotFoundException {
		int n = sin.readInt();
		int m = sin.readInt();
		mb = new ArrayList<ArrayList<STile>>();
		ArrayList<STile> alt;
		for(int i = 0; i < n; ++i) {
			alt = new ArrayList<STile>();
			for (int j = 0; j < m; j++) {
				alt.add((STile) sin.readObject());
			}
			mb.add(alt);
		}
	}
}
