package player;

import gameres.*;

import java.io.*;
import java.util.ArrayList;
import java.util.Random;
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
	 * inicjalizacji pierwszych kilkuset tysięcy słów ze słownika.
	 * 
	 * @param rul Aktywne reguły dla tej rozgrywki.
	 */
	public void simpleInit(SRules rul, BufferedReader dict) throws IOException {
		int size = 200000;
		mwords = new String[size];
		for (int i = 0; i < size; ++i) {
			// dla większego zróżnicowania wczytujemy co dziesiąte słowo
			for(int j = 0; j < 9; ++j) {
				dict.readLine();
			}
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
	 * @param board Aktualna tablica gry.
	 * @param ps Aktualny stan gracza.
	 * @return Ruch wykonany przez gracza.
	 */
	public SAction takeAction(SBoard board, SPlayerState ps) {
		STimeLogger.logStart(Level.INFO, "Kalkulowanie decyzji");
		
		// zamiast wywołania poniższej metody można zaimplementować swoją wypasioną wersję!
		SAction act = takeSimpleAction(board, ps);
		
		STimeLogger.logFinish(Level.INFO, "Decyzja podjęta");
		return act;
	}
	
	/**
	 * Naiwna implementacja logiki gracza.
	 * 
	 * @param board Aktualna plansza gry.
	 * @param ps Aktualny stan gracza.
	 * @return Ruch wykonany przez gracza.
	 */
	public SAction takeSimpleAction(SBoard board, SPlayerState ps) {
		// sprawdzamy czy z literek, ktore mamy, można ułożyc jakieś słowo, 
		// ktore znamy ze słownika
		String word = new String();
		
		for(int i = 0; i < mwords.length; ++i)
		{
			ArrayList<Character> lets = new ArrayList<Character>(ps.letters());
			word = mwords[i];
			int index;
			for(int j = 0; j < word.length(); ++j) {
				index = lets.indexOf(word.charAt(j));
				if(index > 0) {
					// wyraz zawiera literkę, którą mamy
					lets.remove(index);
					if(j == word.length() - 1) {
						// skompletowaliśmy literki do wyrazu
						SAction act = fitSimplyWord(word, board, ps);
						if(act instanceof SActionSkip) {
							continue;
						} else {
							return act;
						}
					}
					continue;
				}
				// nie mamy literki, która znajduje się w wyrazie
				// spróbuj z następnym słowem
				break;
			}
		}
		if (ps.exchanges() < SPlayerState.MAX_EXCHANGES) {
			// spróbuj wymienić kilka literek
			int howMany = mrand.nextInt(ps.letters().size());
			if (howMany > 0) {
				char[] letToEx = new char[howMany];
				for (int i = 0; i < howMany; ++i) {
					letToEx[i] = ps.letters().get(i);
				}
				return new SActionEx(letToEx);
			}
		}
		
		return new SActionSkip();
	}
	
	/**
	 * Próbuje upchać słowo na planszy.
	 * 
	 * @param word Słowo do upchania
	 * @param board Aktualna plansza gry
	 * @param ps Aktualny stan gracza
	 * @return Akcja wstawiania słowa na planszę lub opuszczenia kolejki
	 */
	private SAction fitSimplyWord(String word, SBoard board, SPlayerState ps) {
		// pozycja początkowa pierwszej literki wyrazu na planszy
		// jeśli uda nam się upchać wyraz gdzieś na planszy
		int fitX = -1;
		int fitY = -1;
		// mamy słowo, które teraz spróbujemy gdzieś upchać
		STile[][] tiles = board.tiles();
		
		// jeśli plansza jest pusta to wstawiamy wyraz na jej środek
		boolean isBoardEmpty = true;
		out_loop:
		for(int i = 0; i < tiles.length; ++i) {
			int rowLength = tiles[i].length;
			for(int j = 0; j < rowLength; ++j) {
				if(!tiles[i][j].isEmpty()) {
					isBoardEmpty = false;
					break out_loop;
				}
			}
		}
		if(isBoardEmpty) {
			fitY = tiles.length/2;
			fitX = tiles[fitY].length/2;
		} else {
			// dla uproszczenia sprawdzamy tylko pozycje poziome
			out_loop:
			for(int i = 0; i < tiles.length; ++i) {
				int rowLength = tiles[i].length;
				for(int j = 0; j < rowLength; ++j) {
					if(rowLength - j - 1 < word.length()) {
						// wyraz nie zmieści się w aktualnym wierszu
						// spróbuj z następnym wierszem
						break;
					}
					
					// wyraz musi się przecinać z co najmniej jednym słowem
					boolean atLeastOne = false; 
					// sprawdzamy czy pasuje
					for(int k = 0; k < word.length(); ++k) {
						boolean sameLetter = tiles[i][j + k].letter() == word.charAt(k);
						if(sameLetter) {
							atLeastOne = true;
						}
						if(tiles[i][j + k].isEmpty() || sameLetter) {
							if(k == word.length() - 1 && atLeastOne) {
								// jeśli ostatnia literka w wyrazie to znaczy
								// że udało nam się upchać wyraz na planszy
								fitX = j; 
								fitY = i;
								break out_loop;
							}
							continue;
						}
					}
				}
			}
		}
		
		if(fitX > 0 && fitY > 0 && !isBoardEmpty) {
			// Sprawdzamy czy mamy jakieś sąsiedztwo w otoczeniu słowa.
			// W scruble wszystkie wyrazy na planszy muszą być
			// prawidłowymi słowami, dlatego odrzucamy rozwiązania które
			// tworzą jakieś przypadkowa słowa na planszy
			for(int i = 0; i < word.length(); ++i) {
				// jeśli we wcześniejszym lub kolejnym wierszu znajduje się 
				// jakaś literka w poziomie to porzuć obecne rozwiązanie
				boolean prevNotEmpty = fitY-1 >= 0 && !tiles[fitY-1][fitX + i].isEmpty() && tiles[fitY][fitX + i].isEmpty();
				boolean nextNotEmpty = fitY+1 < tiles.length && !tiles[fitY+1][fitX + i].isEmpty() && tiles[fitY][fitX + i].isEmpty();
				if(prevNotEmpty || nextNotEmpty) {
					fitX = -1;
					fitY = -1;
					break;
				}
			}
			if(fitX > 0 && fitY > 0) {
				// jeśli wcześniej lub później w pionie znajduje się jakieś słowo
				// to porzuć rozwiązanie
				boolean prevNotEmpty = fitX-1 >= 0 && !tiles[fitY][fitX-1].isEmpty();
				boolean nextNotEmpty = fitX+word.length() < tiles[fitY].length && !tiles[fitY][fitX+word.length()].isEmpty();
				if(prevNotEmpty || nextNotEmpty) {
					fitX = -1;
					fitY = -1;
				}
			}
		}
		
		if(fitX > 0 && fitY > 0) {
			// znaleźliśmy miejsce dla słowa, które składa się z literek
			// które mamy oraz nie koliduje w poziomie z innymi słowami
			// konstruujemy odpowiedź do serwera gry
			SActionPut.PutLetter plet;
			ArrayList<SActionPut.PutLetter> plets = new ArrayList<SActionPut.PutLetter>();
			for(int i = 0; i < word.length(); ++i) {
				if(tiles[fitY][fitX + i].isEmpty()) {
					plet = new SActionPut.PutLetter();
					plet.let = word.charAt(i);
					plet.pos = new SPos((short) fitY, (short) (fitX + i));
					plets.add(plet);
				}
			}
			if(plets.size() > 0) {
				return new SActionPut(plets, SActionPut.Orien.HOR);
			} else {
				return new SActionSkip();
			}
		} else {
			return new SActionSkip();
		}
	}
	
	private DataInputStream min;
	private DataOutputStream mout;
	private SRules mrul;
	private BufferedReader mdict;
	private String[] mwords;
	private Random mrand = new Random(System.nanoTime());
}
