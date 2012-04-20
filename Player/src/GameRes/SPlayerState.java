package GameRes;

import java.io.*;
import java.util.ArrayList;

/**
 * Informacje o stanie gry oraz dostępnych zasobach dla gracza.
 * 
 */
public class SPlayerState implements Serializable {
	/**
	 * Ile dokonano wymian liter.
	 */
	public int mexchanges;
	/**
	 * Liczba opuszczonych kolejek.
	 */
	public int mturnsSkipped;
	/**
	 * Unikalny identyfikator gracza.
	 */
	public int mID;
	/**
	 * Liczba zdobytych punktów.
	 */
	public int mpoints;
	/**
	 * Stojak z literkami gracza.
	 */
	ArrayList<Character> mletters;
	
	public SPlayerState() {}

	private void readObject(ObjectInputStream sin) throws IOException,
			ClassNotFoundException {
		mexchanges = sin.readInt();
		mturnsSkipped = sin.readInt();
		mID = sin.readInt();
		mpoints = sin.readInt();
		int n = sin.readInt();
		mletters = new ArrayList<Character>();
		for (int i = 0; i < n; ++i) {
			mletters.add(sin.readChar());
		}
	}
}
