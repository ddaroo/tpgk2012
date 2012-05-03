package gameres;

import java.io.*;

public abstract class SAction {
	public enum ActionId {EX_LETTERS, PUT_LETTERS, SKIP}
	
	abstract public void writeData(DataOutputStream dout) throws IOException;
}
