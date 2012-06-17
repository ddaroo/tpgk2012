package gameres;

import java.io.*;
import java.util.logging.Level;

import main.SApp;

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
		String logmsg = "Akcja - Ex: ";
		dout.writeByte(ActionId.EX_LETTERS.ordinal());
		dout.writeByte(mletters.length);
		for(char let : mletters) {
			byte output = SCharacterCoder.processOutput(let);
			dout.writeByte(output);
			
			logmsg = logmsg
					.concat(Character.toString(let))
					.concat(" ");
		}
		SApp.mlog.log(Level.INFO, logmsg);
	}

	private char[] mletters;
}
