package GameRes;

import java.io.*;
import java.util.Map;

/**
 * Dodatkowe informacje o grze.
 *
 */
public class SRules implements Serializable {
	/**
	 * Możliwe literki i skojarzone z nimi punkty.
	 */
	Map<Character, Integer> mletBonus;
	
	public SRules() {}

	private void readObject(ObjectInputStream sin) throws IOException,
			ClassNotFoundException {
		int n = sin.readInt();
		for(int i = 0; i < n; ++i) {
			mletBonus.put(sin.readChar(), sin.readInt());
		}
	}
}
