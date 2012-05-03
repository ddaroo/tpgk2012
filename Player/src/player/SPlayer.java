package player;

import gameres.*;

import java.io.*;
import java.util.logging.Level;

import main.SApp;
import main.STimeLogger;

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
	 * Metoda wołana przed rozpoczęciem gry. Inicjuje dane ze słownika.
	 * 
	 * @param rul Aktywne reguły dla tej rozgrywki.
	 */
	public void init(SRules rul, BufferedReader dict) throws IOException {
		STimeLogger.logStart(Level.INFO, "Inicjalizacja danych ze słownika");
		mrul = rul;
		mdict = dict;
		
		// zamiast wywołania poniższej metody można zaimplementować swoją wypasioną wersję!
		simpleInit(rul, dict);

		STimeLogger.logFinish(Level.INFO, "Dane zainicjowane");
	}

	/**
	 * Naiwna implementacja inicjalizacji bazy słów ze słownika, polegająca na
	 * inicjalizacji pierwszych kilku tysięcy słów ze słownika.
	 * 
	 * @param rul Aktywne reguły dla tej rozgrywki.
	 */
	public void simpleInit(SRules rul, BufferedReader dict) throws IOException {
		int size = 4000;
		mwords = new String[size];
		for (int i = 0; i < size; ++i) {
			mwords[i] = dict.readLine();
		}
	}
	
	/**
	 * Główna pętla programu, tutaj są pobierane i zwracane dane z serwera 
	 */
	public void playGame(BufferedReader dict) {
		SBoard board;
		SPlayerState ps;
		boolean gameFinished = false;
		int i = 1;
		try {
			// pobierz reguły gry
			SRules rul = new SRules();
			rul.readData(min);
			init(rul, dict); // inicjuj dane do rozgrywki
			while(true) {
				// koniec gry?
				gameFinished = min.readBoolean();
				if(gameFinished) {
					SApp.mlog.log(Level.INFO, "Gra została zakończona po "
								.concat(String.valueOf(i)
								.concat(" turach.")));
					return;
				}
				// dane potrzebne do podjęcia decyzji przez gracza
				board = new SBoard();
				ps = new SPlayerState();
				board.readData(min);
				ps.readData(min);
				// wykonaj ruch
				SAction act = takeAction(board, ps);
				// poinformuj serwer
				act.writeData(mout);
				mout.flush(); // upewnij się, że dane zostały wysłane
				++i;
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
	public SAction takeAction(SBoard boad, SPlayerState ps) {
		STimeLogger.logStart(Level.INFO, "Kalkulowanie decyzji");
		
		// zamiast wywołania poniższej metody można zaimplementować swoją wypasioną wersję!
		SAction act = takeSimpleAction(boad, ps);
		
		STimeLogger.logFinish(Level.INFO, "Decyzja podjęta");
		return act;
	}
	
	/**
	 * Naiwna implementacja logiki gracza.
	 * 
	 * @param boad Aktualna tablica gry.
	 * @param ps Aktualny stan gracza.
	 * @return Ruch wykonany przez gracza.
	 */
	public SAction takeSimpleAction(SBoard boad, SPlayerState ps) {
		// TODO implementation
		return new SActionSkip();
	}
	
	private DataInputStream min;
	private DataOutputStream mout;
	private SRules mrul;
	private BufferedReader mdict;
	private String[] mwords;
}
