package main;

import java.util.logging.Level;

/**
 * Zapisuje do logów informację o początku, zakończeniu obliczeń oraz czasie jaki upłynął.
 */
public class STimeLogger {
	public static void logStart(Level lvl, String msg) {
		SApp.mlog.log(lvl, msg);
		STimeLogger.mstart = System.nanoTime();
	}
	
	/**
	 * 
	 * @return Liczba milisekund, która upłynęła od wywołania logStart()
	 */
	public static long elapsed() {
		return (System.nanoTime() - STimeLogger.mstart) / 1000000;
	}
	
	public static void logFinish(Level lvl, String msg) {
		String elapsed = String.valueOf(STimeLogger.elapsed());
		SApp.mlog.log(
				lvl, 
				msg.concat(", czas = ")
					.concat(elapsed)
					.concat("ms"));
	}
	
	private static long mstart;
}
