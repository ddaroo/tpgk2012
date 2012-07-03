package gameres;

import java.io.*;
import java.util.logging.Level;

import main.SApp;

/**
 * Opuszcza kolejkę
 */
public class SActionSkip extends SAction {
	
	@Override
	public void writeData(DataOutputStream dout) throws IOException {
		dout.writeByte(ActionId.SKIP.ordinal());
		SApp.mlog.log(Level.INFO, "Akcja - Skip");
	}

}
