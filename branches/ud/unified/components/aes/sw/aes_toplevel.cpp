#include <ac_channel.h>
#include "128_aes.h"

enum{
SET_ENCRYPT_KEY = 0xF0,
SET_DECRYPT_KEY = 0xF2,
OP_ENCRYPT = 0xF1,
OP_DECRYPT = 0xF3
};

void topLevel(ac_channel<unsigned char>& din, ac_channel<unsigned char>& dout)
{
	static unsigned char buffer[16];
	static unsigned char op;
	static aesCipher cipher;
	static aesInvCipher invCipher;

	op = din.read();
IN:	for(int i =0;i<16;i++)
	{ 
		buffer[i] = din.read();
	}
	
	switch(op)
	{
		case SET_ENCRYPT_KEY:
			cipher.changeKey(buffer);
			break;
		case SET_DECRYPT_KEY:
			invCipher.changeKey(buffer);
			break;
		case OP_ENCRYPT:
			cipher.encrypt(buffer);
CP_IN:			for(int i=0;i<16;i++)
			{
				dout.write(buffer[i]);
			}
			break;
		case OP_DECRYPT:
			invCipher.decrypt(buffer);
CP_OUT:			for(int i=0;i<16;i++)
			{
				dout.write(buffer[i]);
			}
			break;
		default:
			break;
	}
}

int main()
{
    unsigned char cipher_key[aesKey::CIPHER_SIZE] = // declaration of the key
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

	unsigned char input_text[aesKey::CIPHER_SIZE] = 
	{
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
	};

    unsigned char teste[aesKey::CIPHER_SIZE] = 
    {
       0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30, 0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
    };

	ac_channel<unsigned char> din;
	ac_channel<unsigned char> dout;

	din.write(SET_ENCRYPT_KEY);
READ:		for(int i=0;i<16;i++)
		{
			din.write(cipher_key[i]);
		}

	topLevel(din,dout);

	din.write(OP_ENCRYPT);
WRITE:		for(int i=0;i<16;i++)
		{
			din.write(input_text[i]);
		}
	topLevel(din,dout);

	for(int i=0;i<16;i++)
		{
			input_text[i] = dout.read();
		}




    return 0;
}
