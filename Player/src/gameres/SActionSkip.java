package gameres;

import java.io.*;

/**
 * Opuszcza kolejkę
 */
public class SActionSkip extends SAction {
	
	@Override
	public void writeData(DataOutputStream dout) throws IOException {
		dout.writeByte(ActionId.SKIP.ordinal());
	}

}
