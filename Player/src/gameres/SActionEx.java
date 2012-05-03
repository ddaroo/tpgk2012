package gameres;

import java.io.*;

/**
 * Wymienia literki gracza
 */
public class SActionEx extends SAction {
	/**
	 * 
	 * @param letters Literki do wymiany na nowe
	 */
	public SActionEx(char[] letters) {
		mletters = letters;
	}
	
	@Override
	public void writeData(DataOutputStream dout) throws IOException {
		dout.writeByte(ActionId.EX_LETTERS.ordinal());
		dout.writeByte(mletters.length);
		for(char let : mletters) {
			dout.writeByte(let);
		}
	}

	private char[] mletters;
}
