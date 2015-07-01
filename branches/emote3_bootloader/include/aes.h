#ifndef _AES_H_
#define _AES_H_

// This code was taken from https://github.com/kokke/tiny-AES128-C

#include <utility/cipher.h>

__BEGIN_SYS

class AES: public Cipher
{    
public:
    static const unsigned int Nb = Traits<AES>::Nb;
    static const unsigned int Nk = Traits<AES>::Nk;
    static const unsigned int Nr = Traits<AES>::Nr;
    static const unsigned int KEYLEN = Traits<AES>::KEYLEN;

	AES(){ _mode = ECB; }
	~AES(){}
    void AES128_CBC_encrypt_buffer(char* output, const char* input, int length, const char* key, const char* iv);
    void AES128_CBC_decrypt_buffer(char* output, const char* input, int length, const char* key, const char* iv);
    void AES128_ECB_encrypt(const char* input, const char* key, char *output);
    void AES128_ECB_decrypt(const char* input, const char* key, char *output);

    static const char iv[];//= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	virtual bool mode(Mode m) 
    { 
		bool ok = (m==ECB) || (m==CBC);
		if(ok) _mode = m;
        else db<AES>(ERR) << "AES::mode(" << m <<") ERROR: mode " << m << " not supported" << endl;
		return ok;
    }

	virtual bool encrypt(const char *input_data, const char *key, char *encrypted_data)
    { 
        db<AES>(TRC) << "AES::encrypt" << endl;
        return _crypt(input_data, key, encrypted_data, true);
    }

	virtual bool decrypt(const char *input_data, const char *key, char *decrypted_data)
    { 
        db<AES>(TRC) << "AES::decrypt" << endl;
        return _crypt(input_data, key, decrypted_data, false);
    }

    bool _crypt(const char * data, const char *key, char *result, bool encrypt)
    {
        bool ret = false;
        db<AES>(TRC) << "data = [" << (int)data[0];
        for(unsigned int i=1;i<16;i++)
            db<AES>(TRC) << " ," << (int)data[i];
        db<AES>(TRC) << "]" << endl << "key = [" << (int)key[0];
        for(unsigned int i=1;i<16;i++)
            db<AES>(TRC) << " ," << (int)key[i];
        db<AES>(TRC) << "]" << endl;

        if(_mode == CBC)
        {
            if(encrypt)
                AES128_CBC_encrypt_buffer(result, data, 16, key, iv);
            else
                AES128_CBC_decrypt_buffer(result, data, 16, key, iv);
            ret = true;
        }
        else if(_mode == ECB)
        {
            if(encrypt)
                AES128_ECB_encrypt(data, key, result);
            else
                AES128_ECB_decrypt(data, key, result);
            ret = true;
        }
        if(ret)
        {
            db<AES>(TRC) << "result = [" << (int)result[0];
            for(unsigned int i=1;i<16;i++)
                db<AES>(TRC) << " ," << (int)result[i];
            db<AES>(TRC) << "]" << endl;
        }
        return ret;
    }

    static void XorWithIv(char* buf)
    { for(unsigned int i = 0; i < KEYLEN; ++i) buf[i] ^= Iv[i]; }

private:
    // state - array holding the intermediate results during decryption.
    typedef char state_t[4][4];
    static state_t* state;

    // The array that stores the round keys.
    static char RoundKey[176];

    // The Key input to the AES Program
    static const char* Key;

    // Initial Vector used only for MODE_CBC mode
    static char* Iv;

    static const char sbox[256];
    static const char rsbox[256];
    static const char Rcon[255];

    static char getSBoxValue(int num) { return sbox[num]; }
    static char getSBoxInvert(int num) { return rsbox[num]; }

    static void KeyExpansion(void);
    static void AddRoundKey(int round);
    static void SubBytes(void);
    static void ShiftRows(void);
    static void MixColumns(void);
    static void InvMixColumns(void);
    static void InvSubBytes(void);
    static void InvShiftRows(void);
    static void _Cipher(void);
    static void Inv_Cipher(void);
    static void BlockCopy(char* output, const char* input)
    { for (unsigned int i=0;i<KEYLEN;++i) output[i] = input[i]; }

    static char xtime(char x) { return ((x<<1) ^ (((x>>7) & 1) * 0x1b)); }
    static char Multiply(char x, char y)
    {
        return (((y & 1) * x) ^
                ((y>>1 & 1) * xtime(x)) ^
                ((y>>2 & 1) * xtime(xtime(x))) ^
                ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
                ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))));
    }
};

__END_SYS
#endif
