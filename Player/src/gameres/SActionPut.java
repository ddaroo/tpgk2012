package gameres;

import java.io.*;
import java.util.ArrayList;

/**
 * Wstawia słowo
 */
public class SActionPut extends SAction {
	/**
	 * Orientacja słowa - pionowa albo pozioma
	 */
	public enum Orien {HOR, VER};
	
	/**
	 * Klasa pomocnicza na potrzeby wysłania wymaganych informacji do serwera
	 */
	public static class PutLetter {
		public char let;
		public SPos pos;
		public boolean fromBlank;
		
		public PutLetter() {
			let = '@';
			fromBlank = false;
			pos = new SPos();
		}
	}
	
	public SActionPut(ArrayList<SActionPut.PutLetter> lets, SActionPut.Orien orien) {
		setWord(lets, orien);
	}
	
	public void setWord(ArrayList<SActionPut.PutLetter> lets, SActionPut.Orien orien) {
		mlets = lets;
		morien = orien;
	}

	@Override
	public void writeData(DataOutputStream dout) throws IOException {
		dout.writeByte(ActionId.PUT_LETTERS.ordinal());
		dout.writeByte(morien.ordinal());
		dout.writeByte(mlets.size());
		for(PutLetter let : mlets) {
			dout.writeByte(let.let);
			dout.writeBoolean(let.fromBlank);
			let.pos.writeData(dout);
		}
	}
	
	ArrayList<SActionPut.PutLetter> mlets;
	private SActionPut.Orien morien;
}
