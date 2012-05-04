package gameres;

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
	public STile[][] tiles() {
		return mb;
	}

	public void readData(DataInputStream din) throws IOException {
		short n = din.readShort();
		short m = din.readShort();
		mb = new STile[n][m];
		STile tile;
		for(short i = 0; i < n; ++i) {
			for (short j = 0; j < m; j++) {
				tile = new STile();
				tile.readData(din);
				mb[j][i] = tile;
			}
		}
	}

	private STile[][] mb;
}
