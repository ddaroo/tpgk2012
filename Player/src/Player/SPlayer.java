package Player;

import GameRes.*;
import java.io.*;
import java.util.logging.Level;

import main.SApp;

/**
 * Logika gracza Scrabble.
 *
 */
public class SPlayer {
	public SPlayer(DataInputStream in, DataOutputStream out) {
		min = in;
		mout = out;
	}
	/**
	 * Metoda wołana przed rozpoczęciem gry.
	 * 
	 * @param rul Aktywne reguły dla tej rozgrywki.
	 */
	public void simpleInit(SRules rul) {
		mrul = rul;
		// TODO przygotuj słownik
	}
	
	/**
	 * Główna pętla programu, tutaj są pobierane i zwracane dane z serwera 
	 */
	public void playGame() {
		SBoard board;
		SPlayerState ps;
		boolean gameFinished = false;
		try {
			// pobierz reguły gry
			SRules rul = new SRules();
			rul.readData(min);
			simpleInit(rul); // inicjuj dane do rozgrywki
			while(true) {
				// koniec gry?
				gameFinished = min.readBoolean();
				if(gameFinished) {
					SApp.mlog.log(Level.INFO, "Gra została zakończona");
					return;
				}
				// dane potrzebne do podjęcia decyzji przez gracza
				board = new SBoard();
				ps = new SPlayerState();
				board.readData(min);
				ps.readData(min);
				// wykonaj ruch
				SAction act = takeSimpleAction(board, ps);
				// poinformuj serwer
				act.writeData(mout);
				mout.flush(); // upewnij się, że dane zostały wysłane
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Metoda jest wołana każdorazowo, gdy serwer zarząda wykonania ruchu przez
	 * gracza. Parametry funkcji są przekazywane bezpośrednio z serwera.
	 * 
	 * @param boad Aktualna tablica gry.
	 * @param ps Aktualny stan gracza.
	 * @return Ruch wykonany przez gracza.
	 */
	public SAction takeSimpleAction(SBoard boad, SPlayerState ps) {
		// TODO implement
		return new SAction();
	}
	
	private DataInputStream min;
	private DataOutputStream mout;
	private SRules mrul;
}
