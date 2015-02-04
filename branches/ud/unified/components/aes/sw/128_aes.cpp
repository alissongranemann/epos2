#include "128_aes.h"

const unsigned char aesKey::sBox[256] = //Byte substitution box pre calculated
{
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

const unsigned char aesKey::inv_sBox[256] = //inverse sBox also pre-calculated
{
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

const unsigned char aesKey::Rcon[11] = //Round constant array pre calculated (only 11 rounds in expandKey)
	{0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};


//************************************ BEGIN AES KEY ************************************
void aesKey::expandKey(unsigned char* key_in)
{
	unsigned char temp;
CPY:    for(int i = 0; i< 16; i++)
    {
       key_schedule[i] = key_in[i]; 
    }
EXPK:	for(int i = 1;i < 11; i++)
	{
		temp = key_schedule[i*16 - 4];
		key_schedule[i*16 + 0] = sBox[key_schedule[i*16 - 3]]^key_schedule[(i-1)*16 + 0]^Rcon[i];
		key_schedule[i*16 + 1] = sBox[key_schedule[i*16 - 2]]^key_schedule[(i-1)*16 + 1];
		key_schedule[i*16 + 2] = sBox[key_schedule[i*16 - 1]]^key_schedule[(i-1)*16 + 2];
		key_schedule[i*16 + 3] = sBox[temp]^key_schedule[(i-1)*16 + 3];

		key_schedule[i*16 + 4] = key_schedule[(i-1)*16 + 4]^key_schedule[i*16 + 0];
		key_schedule[i*16 + 5] = key_schedule[(i-1)*16 + 5]^key_schedule[i*16 + 1];
		key_schedule[i*16 + 6] = key_schedule[(i-1)*16 + 6]^key_schedule[i*16 + 2];
		key_schedule[i*16 + 7] = key_schedule[(i-1)*16 + 7]^key_schedule[i*16 + 3];
		key_schedule[i*16 + 8] = key_schedule[(i-1)*16 + 8]^key_schedule[i*16 + 4];
		key_schedule[i*16 + 9] = key_schedule[(i-1)*16 + 9]^key_schedule[i*16 + 5];
		key_schedule[i*16 + 10] = key_schedule[(i-1)*16 + 10]^key_schedule[i*16 + 6];
		key_schedule[i*16 + 11] = key_schedule[(i-1)*16 + 11]^key_schedule[i*16 + 7];
		key_schedule[i*16 + 12] = key_schedule[(i-1)*16 + 12]^key_schedule[i*16 + 8];
		key_schedule[i*16 + 13] = key_schedule[(i-1)*16 + 13]^key_schedule[i*16 + 9];
		key_schedule[i*16 + 14] = key_schedule[(i-1)*16 + 14]^key_schedule[i*16 + 10];
		key_schedule[i*16 + 15] = key_schedule[(i-1)*16 + 15]^key_schedule[i*16 + 11];
	}
}		

unsigned char aesKey::gf2(unsigned char value)
{
	if (value>>7)
	{
		value = value << 1;
		return (value^0x1b);
	} else
		return value<<1;
}

void aesKey::addRoundKey(unsigned char* state, unsigned char* cipherKey, unsigned char round) // state
{
ADDRK:	for(int count = 0; count < CIPHER_SIZE; count++)
	{
		state[count] ^= cipherKey[(CIPHER_SIZE)*round + count]; // takes the round key, pre calculated in keyExpansion, and XOR with the state array
	}

}

void aesKey::changeKey(unsigned char* new_key){
    expandKey(new_key);
}
//************************************ END AES KEY ************************************

//************************************ BEGIN AES CIPHER ************************************

void aesCipher::mixColumns(unsigned char* in) //Mix collumn operation in = state
{
	unsigned char temp[16]; // i can unroll the loop if temp = 16
	
MIX:	for(int i = 0; i < NB; i++)
	{
		temp[(i*4) + 0] = in[(i*4) + 0] ^ in[(i*4) + 1] ^ in[(i*4) + 2] ^ in[(i*4) + 3];
		temp[(i*4) + 1] = in[(i*4) + 0];
		
		temp[(i*4) + 2] = in[(i*4) + 0] ^ in[(i*4) + 1]; 
		temp[(i*4) + 2] = gf2(temp[(i*4) + 2]);
		in[(i*4) + 0] = in[(i*4) + 0] ^ temp[(i*4) + 2] ^ temp[(i*4) + 0];

		temp[(i*4) + 2] = in[(i*4) + 1] ^ in[(i*4) + 2]; 
		temp[(i*4) + 2] = gf2(temp[(i*4) + 2]);
		in[(i*4) + 1] = in[(i*4) + 1] ^ temp[(i*4) + 2] ^ temp[(i*4) + 0];

		temp[(i*4) + 2] = in[(i*4) + 2] ^ in[(i*4) + 3]; 
		temp[(i*4) + 2] = gf2(temp[(i*4) + 2]);
		in[(i*4) + 2] = in[(i*4) + 2] ^ temp[(i*4) + 2] ^ temp[(i*4) + 0];

		temp[(i*4) + 2] = in[(i*4) + 3] ^ temp[(i*4) + 1]; 
		temp[(i*4) + 2] = gf2(temp[(i*4) + 2]);
		in[(i*4) + 3] = in[(i*4) + 3] ^ temp[(i*4) + 2] ^ temp[(i*4) + 0];

	}
}

void aesCipher::shiftRows(unsigned char* in) // permutation step, in = state 
{
	unsigned char t;

	t = in[1];
	in[1] = in[5];
	in[5] = in[9];
	in[9] = in[13];
	in[13] = t;

	t = in[2];
	in[2] = in[10];
	in[10] = t;

	t = in[6];
	in[6] = in[14];
	in[14] = t;

	t = in[3];
	in[3] = in[15];
	in[15] = in[11];
	in[11] = in[7];
	in[7] = t; 
}

void aesCipher::subBytes(unsigned char* in)//in = state, confusion step
{
SUB:	for(int i=0; i<CIPHER_SIZE;i++)
	{
		in[i] = sBox[in[i]]; // takes the correspondent value from the Sbox Array
	}
}

void aesCipher::cipher(unsigned char in[4*NB], unsigned char* key_in) //in = plain text, out = encrypted text, key_schedule = round key array
{
	unsigned char* state; // state variable 
	//unsigned char* out; //output (not nescessary, since it uses a pointer to the inpt array array)
	unsigned char round = 0; // round counter  
	state = in; // state recieves the input

	addRoundKey(state, key_in, round); // starts by adding the round key to the array

ROUND:	for(round = 1; round < 10; round++) // for the next 9 rounds
	{
		subBytes(state); // confuion
		shiftRows(state); // permutation
		mixColumns(state); // difusion       
		addRoundKey(state, key_in, round); // key secrecy
	}
	// ommits mix collumn operation in the last round
	subBytes(state);
	shiftRows(state);
	addRoundKey(state, key_in, round);
}

void aesCipher::encrypt(unsigned char* plain_text){
	cipher(plain_text, key_schedule);
}	
//************************************ END AES CIPHER ************************************


//************************************ BEGIN INVERSE AES CIPHER ************************************
void aesInvCipher::invSubBytes(unsigned char* in)//in = state, confusion step
{
ISUB:	for(int i=0; i<CIPHER_SIZE;i++)
	{
		in[i] = inv_sBox[in[i]]; // takes the correspondent value from the invSbox Array
	}
}

void aesInvCipher::invShiftRows(unsigned char* in)
{
	unsigned char t;
	t = in[13];
	in[13] = in[9];
	in[9] = in[5];
	in[5] = in[1];
	in[1] = t;

	t = in[2];
	in[2] = in[10];
	in[10] = t;

	t = in[6];
	in[6] = in[14];
	in[14] = t;

	t = in[3];
	in[3] = in[7];
	in[7] = in[11];
	in[11] = in[15];
	in[15] = t; 
}

void aesInvCipher::invMixColumns(unsigned char* state)
{
	unsigned char buf1,buf2,buf3;

	buf1 = gf2(gf2(state[0]^state[2]));
	buf2 = gf2(gf2(state[1]^state[3]));
	state[0] ^= buf1;     state[1] ^= buf2;    state[2] ^= buf1;    state[3] ^= buf2;
	//col2
	buf1 = gf2(gf2(state[4]^state[6]));
	buf2 = gf2(gf2(state[5]^state[7]));
	state[4] ^= buf1;    state[5] ^= buf2;    state[6] ^= buf1;    state[7] ^= buf2;
	//col3
	buf1 = gf2(gf2(state[8]^state[10]));
	buf2 = gf2(gf2(state[9]^state[11]));
	state[8] ^= buf1;    state[9] ^= buf2;    state[10] ^= buf1;    state[11] ^= buf2;
	//col4
	buf1 = gf2(gf2(state[12]^state[14]));
	buf2 = gf2(gf2(state[13]^state[15]));
	state[12] ^= buf1;    state[13] ^= buf2;    state[14] ^= buf1;    state[15] ^= buf2;

	// col1
	buf1 = state[0] ^ state[1] ^ state[2] ^ state[3];
	buf2 = state[0];
	buf3 = state[0]^state[1]; buf3=gf2(buf3); state[0] = state[0] ^ buf3 ^ buf1;
	buf3 = state[1]^state[2]; buf3=gf2(buf3); state[1] = state[1] ^ buf3 ^ buf1;
	buf3 = state[2]^state[3]; buf3=gf2(buf3); state[2] = state[2] ^ buf3 ^ buf1;
	buf3 = state[3]^buf2;     buf3=gf2(buf3); state[3] = state[3] ^ buf3 ^ buf1;
	// col2
	buf1 = state[4] ^ state[5] ^ state[6] ^ state[7];
	buf2 = state[4];
	buf3 = state[4]^state[5]; buf3=gf2(buf3); state[4] = state[4] ^ buf3 ^ buf1;
	buf3 = state[5]^state[6]; buf3=gf2(buf3); state[5] = state[5] ^ buf3 ^ buf1;
	buf3 = state[6]^state[7]; buf3=gf2(buf3); state[6] = state[6] ^ buf3 ^ buf1;
	buf3 = state[7]^buf2;     buf3=gf2(buf3); state[7] = state[7] ^ buf3 ^ buf1;
	// col3
	buf1 = state[8] ^ state[9] ^ state[10] ^ state[11];
	buf2 = state[8];
	buf3 = state[8]^state[9];   buf3=gf2(buf3); state[8] = state[8] ^ buf3 ^ buf1;
	buf3 = state[9]^state[10];  buf3=gf2(buf3); state[9] = state[9] ^ buf3 ^ buf1;
	buf3 = state[10]^state[11]; buf3=gf2(buf3); state[10] = state[10] ^ buf3 ^ buf1;
	buf3 = state[11]^buf2;      buf3=gf2(buf3); state[11] = state[11] ^ buf3 ^ buf1;
	// col4
	buf1 = state[12] ^ state[13] ^ state[14] ^ state[15];
	buf2 = state[12];
	buf3 = state[12]^state[13]; buf3=gf2(buf3); state[12] = state[12] ^ buf3 ^ buf1;
	buf3 = state[13]^state[14]; buf3=gf2(buf3); state[13] = state[13] ^ buf3 ^ buf1;
	buf3 = state[14]^state[15]; buf3=gf2(buf3); state[14] = state[14] ^ buf3 ^ buf1;
	buf3 = state[15]^buf2;      buf3=gf2(buf3); state[15] = state[15] ^ buf3 ^ buf1;  
	
}

void aesInvCipher::invCipher(unsigned char in[4*NB], unsigned char* key_in) //in = encrypted text, out = plain text, key schedule = round key array
{
	unsigned char* state; // state variable 
	//unsigned char* out; //output (not nescessary, since it uses a pointer to the inpt array array)
	unsigned char round = 10; // round counter  
	state = in; // state recieves the input

	addRoundKey(state, key_in, round);

IROUND:	for(round = 9; round > 0; round--) // for the next 9 rounds
	{
		invShiftRows(state); // permutation
		invSubBytes(state); // confuion
		addRoundKey(state, key_in ,round); // we need to call addRoundKey with the inverse number of rounds
		invMixColumns(state); // difusion
	}
	// ommits mix collumn operation in the last round
	invShiftRows(state);    
	invSubBytes(state);    
	addRoundKey(state, key_in, round);
}

void aesInvCipher::decrypt(unsigned char* cipher_text)
{
	invCipher(cipher_text, key_schedule);
}
//************************************ END INVERSE AES CIPHER ************************************
