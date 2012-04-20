package GameRes;

import java.io.*;

/**
 * Pjedyńcza płytka do gry.
 *
 */
public class STile implements Serializable {
	public SPos mpos;
	public char mletter;
	
	public enum BonusType { NONE, LETTER, WORD };
	/**
	 * Typ bonusu - do literki lub całego słowa.
	 */
	public BonusType mbonus;
	/**
	 * Wartość bonusu punktowego.
	 */
	public int mbonusVal;
	
	public STile() {
		mletter = STile.empty;
	}
	
	boolean isEmpty() {
		return mletter == STile.empty;
	}

	private void readObject(ObjectInputStream sin) throws IOException,
			ClassNotFoundException {
		mpos = (SPos) sin.readObject();
		mletter = sin.readChar();
		// TODO bonusType
		mbonus = null;
		mbonusVal = sin.readInt();
	}
	// TODO const
	private static char empty = '@';
}