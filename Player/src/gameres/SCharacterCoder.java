package gameres;

import java.util.HashMap;
import java.util.Map;

public class SCharacterCoder {
	private static final Map<Byte, Character> specialCharactersMap = new HashMap<Byte, Character>();
	static {
		specialCharactersMap.put(Byte.valueOf((byte) 0xb9), 'ą');
		specialCharactersMap.put(Byte.valueOf((byte) 0xea), 'ę');
		specialCharactersMap.put(Byte.valueOf((byte) 0xe6), 'ć');
		specialCharactersMap.put(Byte.valueOf((byte) 0x9f), 'ź');
		specialCharactersMap.put(Byte.valueOf((byte) 0xf1), 'ń');
		specialCharactersMap.put(Byte.valueOf((byte) 0xf3), 'ó');
		specialCharactersMap.put(Byte.valueOf((byte) 0xb3), 'ł');
		specialCharactersMap.put(Byte.valueOf((byte) 0xbf), 'ż');
		specialCharactersMap.put(Byte.valueOf((byte) 0x9c), 'ś');
		
		// Typowe znaki
		specialCharactersMap.put(Byte.valueOf((byte) 0x61), 'a');
		specialCharactersMap.put(Byte.valueOf((byte) 0x62), 'b');
		specialCharactersMap.put(Byte.valueOf((byte) 0x63), 'c');
		specialCharactersMap.put(Byte.valueOf((byte) 0x64), 'd');
		specialCharactersMap.put(Byte.valueOf((byte) 0x65), 'e');
		specialCharactersMap.put(Byte.valueOf((byte) 0x66), 'f');
		specialCharactersMap.put(Byte.valueOf((byte) 0x67), 'g');
		specialCharactersMap.put(Byte.valueOf((byte) 0x68), 'h');
		specialCharactersMap.put(Byte.valueOf((byte) 0x69), 'i');
		specialCharactersMap.put(Byte.valueOf((byte) 0x6a), 'j');
		specialCharactersMap.put(Byte.valueOf((byte) 0x6b), 'k');
		specialCharactersMap.put(Byte.valueOf((byte) 0x6c), 'l');
		specialCharactersMap.put(Byte.valueOf((byte) 0x6d), 'm');
		specialCharactersMap.put(Byte.valueOf((byte) 0x6e), 'n');
		specialCharactersMap.put(Byte.valueOf((byte) 0x6f), 'o');
		specialCharactersMap.put(Byte.valueOf((byte) 0x70), 'p');
		specialCharactersMap.put(Byte.valueOf((byte) 0x71), 'q');
		specialCharactersMap.put(Byte.valueOf((byte) 0x72), 'r');
		specialCharactersMap.put(Byte.valueOf((byte) 0x73), 's');
		specialCharactersMap.put(Byte.valueOf((byte) 0x74), 't');
		specialCharactersMap.put(Byte.valueOf((byte) 0x75), 'u');
		specialCharactersMap.put(Byte.valueOf((byte) 0x76), 'v');
		specialCharactersMap.put(Byte.valueOf((byte) 0x77), 'w');
		specialCharactersMap.put(Byte.valueOf((byte) 0x78), 'x');
		specialCharactersMap.put(Byte.valueOf((byte) 0x79), 'y');
		specialCharactersMap.put(Byte.valueOf((byte) 0x7a), 'z');
	}

	private static final Map<Character, Byte> specialCharactersReverseMap = new HashMap<Character, Byte>();
	static {
		for(Map.Entry<Byte, Character> entry : specialCharactersMap.entrySet()) {
			specialCharactersReverseMap.put(entry.getValue(), entry.getKey());
		}
	}

	public static Character processInput(Byte b) {
		Character input = specialCharactersMap.get(b);
		if(input == null) {
			input = Character.valueOf((char) b.byteValue());  
		}
		return input;
	}
	
	public static byte processOutput(Character c) {
		Byte output = specialCharactersReverseMap.get(c);
		if(output == null) {
			output = Byte.valueOf((byte) c.charValue()); 
		}
		return output;
	}
	
}

