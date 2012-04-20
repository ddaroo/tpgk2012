package Player;

import GameRes.*;

/**
 * Logika gracza Scrabble.
 *
 */
public class SPlayer {
	public SPlayer() {}
	/**
	 * Metoda wołana przed rozpoczęciem gry.
	 * 
	 * @param rul Aktywne reguły dla tej rozgrywki.
	 */
	public void init(SRules rul) {
		
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
		// TODO implement
		return new SAction();
	}
}
