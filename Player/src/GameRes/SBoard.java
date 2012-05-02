package GameRes;

import java.io.*;
import java.util.ArrayList;

/**
 * Plansza do gry Scrabble.
 */
public class SBoard {
	// TODO implementation
	public SBoard() {}
	
	/**
	 * @return Płytki na planszy
	 */
	public ArrayList<ArrayList<STile>> tiles() {
		return mb;
	}

	public void readData(DataInputStream din) throws IOException {
		short n = din.readShort();
		short m = din.readShort();
		mb = new ArrayList<ArrayList<STile>>();
		ArrayList<STile> alt;
		STile tile;
		for(short i = 0; i < n; ++i) {
			alt = new ArrayList<STile>();
			for (short j = 0; j < m; j++) {
				tile = new STile();
				tile.readData(din);
				alt.add(tile);
			}
			mb.add(alt);
		}
	}

	private ArrayList<ArrayList<STile>> mb;
}
