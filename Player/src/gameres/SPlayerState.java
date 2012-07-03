package gameres;

import java.io.*;
import java.util.ArrayList;

/**
 * Informacje o stanie gry oraz dostępnych zasobach dla gracza.
 */
public class SPlayerState {
	public static final int MAX_EXCHANGES = 3;
	
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
		setMletters(new ArrayList<Character>());
		for (short i = 0; i < n; ++i) {
			byte input = din.readByte();
			Character letter = SCharacterCoder.processInput(input);
			getMletters().add(letter);
		}
	}

	public ArrayList<Character> getMletters() {
		return mletters;
	}
	
	public ArrayList<Character> getMlettersCopy() {
		ArrayList<Character> copy = new ArrayList<Character>();
		for (Character character : this.mletters) {
			copy.add(character.charValue());
		}
		return copy;
	}

	public void setMletters(ArrayList<Character> mletters) {
		this.mletters = mletters;
	}
	
	private short mexchanges;
	private short mturnsSkipped;
	 // Unikalny identyfikator gracza.
	private short mID;
	private short mpoints;
	private ArrayList<Character> mletters;
}
