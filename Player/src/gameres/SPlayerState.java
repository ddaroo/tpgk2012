package gameres;

import java.io.*;
import java.util.ArrayList;

/**
 * Informacje o stanie gry oraz dostępnych zasobach dla gracza.
 */
public class SPlayerState {
	public SPlayerState() {}
	
	/**
	 * @return Ile dokonano wymian liter
	 */
	public int exchanges() {
		return mexchanges;
	}
	
	/**
	 * @return Liczba opuszczonych kolejek
	 */
	public int turnsSkipped() {
		return mturnsSkipped;
	}
	
	/**
	 * @return Liczba zdobytych punktów
	 */
	public int points() {
		return mpoints;
	}
	
	/**
	 * @return Aktualnie trzymane litery 
	 */
	public ArrayList<Character> letters() {
		return mletters;
	}

	public void readData(DataInputStream din) throws IOException {
		mexchanges = din.readShort();
		mturnsSkipped = din.readShort();
		mID = din.readShort();
		mpoints = din.readShort();
		short n = din.readShort();
		mletters = new ArrayList<Character>();
		for (short i = 0; i < n; ++i) {
			mletters.add((char) din.readByte());
		}
	}

	private short mexchanges;
	private short mturnsSkipped;
	 // Unikalny identyfikator gracza.
	private short mID;
	private short mpoints;
	private ArrayList<Character> mletters;
}
