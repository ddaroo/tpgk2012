package GameRes;

import java.io.*;

public class SAction {
	public SAction() {}
	
	public void writeData(DataOutputStream dout) throws IOException {
		dout.writeByte(1);
	}
}
