/** 
 *  This is an implementation of the AES algorithm 
 *  This program compares the performance of running algorithm on fpga and regular CPU
 *
 *  @author Minglun Zhang
 *  @author Zhihang Yuan
 *  
 *  Source cited: https://www.ijera.com/papers/Vol3_issue1/IW3116621670.pdf
 *                https://github.com/kokke/tiny-AES-c/blob/master/aes.c
 *                http://aes.online-domain-tools.com/
 *                https://www.youtube.com/watch?v=evjFwDRTmV0
 *                https://en.wikipedia.org/wiki/Advanced_Encryption_Standard
 *                http://gauss.ececs.uc.edu/Courses/c6053/extra/AES/mixcolumns.cpp
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "aes.h"
// We use round number 10 for AES 128
#define ROUND 10
// The bytes of every message
#define MAX_WIDTH 16
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))

// The Byte Substitution Box
unsigned char sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 
};

// The Rcon Matrix
unsigned char Rcon[ROUND + 1] = {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

// The reverse Substitution Box
unsigned char rsbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d 
};

/**
 * Key schedule helper function
 */
void keyExpansionCore (unsigned char* in, unsigned char i) {
    unsigned int* q = (unsigned int*) in;
    
    *q  = (*q >> 8) | ((*q & 0xff) << 24);

    in[0] = sbox[in[0]];
    in[1] = sbox[in[1]];
    in[2] = sbox[in[2]];
    in[3] = sbox[in[3]];

    in[0] ^= Rcon[i];
}

/**
 * The key schedule function
 * Expand the key into the size for round number
 */
void keyExpansion (unsigned char* inputKey, unsigned char* expansionKeys) {
    for(int i = 0; i < MAX_WIDTH; i++){
        expansionKeys[i] = inputKey[i];
    }
    int byteG = MAX_WIDTH;
    int rcon = 1;
    unsigned char temp[4];

    while (byteG < 176) {
        for (int i = 0; i < 4; i++) {
            temp[i] = expansionKeys[i + byteG - 4];
        }
        if (byteG % MAX_WIDTH == 0) {
            keyExpansionCore(temp, rcon);
            rcon++;
        }
        for (unsigned char a = 0; a < 4; a++) {
            expansionKeys[byteG] = expansionKeys[byteG - MAX_WIDTH] ^ temp[a];
            byteG++;
        }
    }
}

/**
 * Using Substitution Box to find the substitution value
 */
void subBytes (unsigned char* state) {
    for (int i = 0; i < MAX_WIDTH; i++) {
        state[i] = sbox[state[i]];
    }
}

/**
 * Using reverse Substitution Box to find the substitution value
 */
void invSubBytes (unsigned char* state) {
    for (int i = 0; i < MAX_WIDTH; i++) {
        state[i] = rsbox[state[i]];
    }
}

/**
 * Shift each row of the state
 */
void shiftRows (unsigned char* state) {
    unsigned char temp[MAX_WIDTH];
    temp[0] = state[0];
    temp[1] = state[5];
    temp[2] = state[10];
    temp[3] = state[15];

    temp[4] = state[4];
    temp[5] = state[9];
    temp[6] = state[14];
    temp[7] = state[3];

    temp[8] = state[8];
    temp[9] = state[13];
    temp[10] = state[2];
    temp[11] = state[7];

    temp[12] = state[12];
    temp[13] = state[1];
    temp[14] = state[6];
    temp[15] = state[11];

    for (int i = 0; i < MAX_WIDTH; i++) {
        state[i] = temp[i];
    }
}

/**
 * Back shift each row of the state
 */
void invShiftRows(unsigned char* state) {
    unsigned char temp[MAX_WIDTH];
    temp[0] = state[0];
    temp[1] = state[13];
    temp[2] = state[10];
    temp[3] = state[7];

    temp[4] = state[4];
    temp[5] = state[1];
    temp[6] = state[14];
    temp[7] = state[11];

    temp[8] = state[8];
    temp[9] = state[5];
    temp[10] = state[2];
    temp[11] = state[15];

    temp[12] = state[12];
    temp[13] = state[9];
    temp[14] = state[6];
    temp[15] = state[3];

    for (int i = 0; i < MAX_WIDTH; i++) {
        state[i] = temp[i];
    }
}

unsigned char multiply (unsigned char x, unsigned char y) {    
   unsigned char c = 0;
   unsigned char d = y;
   for (int i = 0; i < MAX_WIDTH / 2; i++) {
      if (x % 2 == 1) {
      	c ^= d;
      }
      x /= 2;
      d = xtime(d);
   }
   return c;
}


void mixColumns (unsigned char* state) {
  unsigned char a, b, c, d, temp, foo;
  for (int i = 0; i < 4; i++) {  
    a = state[4 * i + 0];
    b = state[4 * i + 1];
    c = state[4 * i + 2];
    d = state[4 * i + 3];

    temp = a ^ b ^ c ^ d;

    foo = a ^ b;
    foo = xtime(foo);
    state[4 * i + 0] ^= foo ^ temp;

    foo = b ^ c;
    foo = xtime(foo);
    state[4 * i + 1] ^= foo ^ temp;

    foo = c ^ d;
    foo = xtime(foo);
    state[4 * i + 2] ^= foo ^ temp;

    foo = d ^ a;
        foo = xtime(foo);

    state[4 * i + 3] ^= foo ^ temp;
  }
}

void invMixColumns (unsigned char* state) {
  unsigned char a, b, c, d;
  for (int i = 0; i < 4; i++) {  
    a = state[4 * i + 0];
    b = state[4 * i + 1];
    c = state[4 * i + 2];
    d = state[4 * i + 3];

    state[4 * i + 0] = multiply(a, 0x0e) ^ multiply(b, 0x0b) ^ multiply(c, 0x0d) ^ multiply(d, 0x09);
    state[4 * i + 1] = multiply(a, 0x09) ^ multiply(b, 0x0e) ^ multiply(c, 0x0b) ^ multiply(d, 0x0d);
    state[4 * i + 2] = multiply(a, 0x0d) ^ multiply(b, 0x09) ^ multiply(c, 0x0e) ^ multiply(d, 0x0b);
    state[4 * i + 3] = multiply(a, 0x0b) ^ multiply(b, 0x0d) ^ multiply(c, 0x09) ^ multiply(d, 0x0e);

  }
}

void addRoundKey (unsigned char* state, unsigned char* roundKey) {
    for (int i = 0; i < MAX_WIDTH; i++) {
        state[i] ^= roundKey[i];
    }
}

void encryption (unsigned char* state, unsigned char* key) {
    // the final round does not include the mixColumns transformation
    addRoundKey(state, key);
    for (int i = 0; i < ROUND - 1; i++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, key + (MAX_WIDTH * (i + 1)));
    }
    subBytes(state);
    shiftRows(state); 
    addRoundKey(state, key + MAX_WIDTH * ROUND);
}


void decryption (unsigned char* state, unsigned char* key) {
    // the inverse order of encryption
    // the final round does not include the mixColumns transformation
    addRoundKey(state, key + MAX_WIDTH * ROUND);
    for (int i = ROUND - 2; i >= 0; i--) {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, key + (MAX_WIDTH * (i + 1)));
        invMixColumns(state);
    }
    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, key);
}

void encrypt (int lines, unsigned char* state, unsigned char* key) {
    for (int i = 0; i < lines; i++) {
        encryption(state + i * MAX_WIDTH, key);
    }
}

void decrypt (int lines, unsigned char* state, unsigned char* key) {
    for (int i = 0; i < lines; i++) {
        decryption(state + i * MAX_WIDTH, key);
    }
}

int main (int argc, char *argv[]) {
    FILE *fp;
    int mode = 0;
    int numberOfLines;
    int size;
    int num_bytes_read;
    unsigned char *message;
    if (argc != 4)
    {
        fprintf(stderr,"Usage: %s input_file number_of_lines mode\n",argv[0]);        
    }
    numberOfLines = atoi(argv[2]);
    mode = atoi(argv[3]);
    fp = fopen(argv[1],"r");
    if (fp == NULL) {
        fprintf(stderr,"Cannot open %s\n",argv[1]);
        exit(EXIT_FAILURE);
    }
    size = numberOfLines * sizeof(unsigned char) * 16;
    message = (unsigned char*)malloc(size);
    num_bytes_read = fread(message,sizeof(unsigned char),numberOfLines * 16,fp);
    fclose(fp);

    unsigned char key[MAX_WIDTH] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    unsigned char expandedKey[MAX_WIDTH * (ROUND + 1)];
    keyExpansion(key, expandedKey);
    switch(mode){
        case 0:
            encrypt(numberOfLines, message, expandedKey);
            printf("Encryption: \n");
            printf("%s\n", message);
            break;
        case 1:
            decrypt(numberOfLines, message, expandedKey);
            printf("Decryption: \n");
            printf("%s\n", message);
            break;
            
        case 2:
            printf("FPGA Encryption: \n");
            encryption_fpga(numberOfLines, message, key);
            for (int i = 0; i < MAX_WIDTH; i++) {
                printf("%02x ", message[i]);
            }
            break;
        default:
            decryption_fpga(numberOfLines, message, key);
            printf("\nFPGA Decryption: \n");
            break;
    }
    return 0;
}
