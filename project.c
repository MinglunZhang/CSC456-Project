#define ROUND 10
#define S-BOX []
#define RCON [] for key schedule
some matrix ???? for mix columns

void keySchedule(key) {
	
}

void subBytes(dongxi) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			dongxi[j*4+i] = S-BOX[dongxi[j*4+i]];
		}
	}
}

void shiftRows(text) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			text[i][j] = 
		}
	}
}

void mixColumns() {
	
}

void addRoundKey(text, key) {
	// the key schedule
	keySchedule(key);


}

void encryption(text, key) {
	
	// the final round does not include the mixColumns transformation
	addRoundKey();
	for (int i = 0; i < ROUND - 1; i++) {
		subBytes(text);
		shiftRowS(text);
		mixColumns(text);
		addRoundKey(text, key);
	}
	subBytes(text);
	shiftRows(text);
	addRoundKey(text, key);
}

void decryption(text, key) {
	
}