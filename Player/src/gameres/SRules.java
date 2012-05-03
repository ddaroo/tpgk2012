package gameres;

import java.io.*;
import java.util.*;

/**
 * Dodatkowe informacje o grze.
 */
public class SRules {
	public SRules() {
		mletBonus = new HashMap<Character, Short>();
	}
	
	/**
	 * Niektóre literki są wyżej punktowane.
	 * 
	 * @return Możliwe literki i skojarzone z nimi punkty
	 */
	public Map<Character, Short> letterBonuses() {
		return mletBonus;
	}

	public void readData(DataInputStream din) throws IOException {
		short n = din.readShort();
		for(short i = 0; i < n; ++i) {
			char ch = (char) din.readByte();
			short val = din.readShort();
			mletBonus.put(ch, val);
		}
	}
	
	private Map<Character, Short> mletBonus;
}
