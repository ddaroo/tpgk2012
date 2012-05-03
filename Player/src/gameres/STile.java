package gameres;

import java.io.*;

/**
 * Pojedyńcza płytka do gry.
 */
public class STile {	
	public enum BonusType { NONE, LETTER, WORD };
	
	public STile() {
		mlet = STile.empty;
	}
	
	/**
	 * @return Typ bonusu - do literki lub całego słowa
	 */
	public BonusType bonusType() {
		return mbonusType;
	}
	
	/**
	 * @return Wartość bonusu punktowego
	 */
	public short bonus() {
		return mbonus;
	}
	
	/**
	 * @return Pozycja płytki na planszy
	 */
	public SPos position() {
		return mpos;
	}
	
	/**
	 * @return Przechowywana literka
	 */
	public char letter() {
		return mlet;
	}
	
	public boolean isEmpty() {
		return mlet == STile.empty;
	}

	public void readData(DataInputStream din) throws IOException {
		mpos = new SPos();
		mpos.readData(din);
		mlet = (char) din.readByte();
		int btype =  din.readByte();
		switch(btype) {
		case 0:
			mbonusType = BonusType.NONE;
			break;
		case 1:
			mbonusType = BonusType.LETTER;
			break;
		case 2:
			mbonusType = BonusType.WORD;
			break;
		default:
				throw new IOException();
		}
		mbonus = din.readShort();
	}

	private SPos mpos;
	private char mlet;
	
	private BonusType mbonusType;
	private short mbonus;

	private static final char empty = '@';
}