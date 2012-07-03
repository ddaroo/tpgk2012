package gameres;

import java.io.*;

/**
 * Plansza do gry Scrabble.
 */
public class SBoard {

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
				
				if(tile.position().x() != j || tile.position().y() != i)
					throw new IOException("Błąd serializacji pozycji płytek na planszy");
			}
		}
	}

	private STile[][] mb;
}
