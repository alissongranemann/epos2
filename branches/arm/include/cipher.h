// EPOS Cipher Mediator Common Package

#ifndef __cipher_h
#define __cipher_h

#include <system/config.h>

__BEGIN_SYS

class Cipher_Common
{
public:
	enum Mode {
		ECB,
		CBC,
	};

protected:
    Cipher_Common() {}
};

class Xor_Cipher: public Cipher_Common {
public:
    static const unsigned int KEY_SIZE = Traits<Xor_Cipher>::KEY_SIZE;

	static bool encrypt(const char *input, const char *key, char *encrypted_data) {
        return crypt(input, key, encrypted_data, true);
    }

	static bool decrypt(const char *input, const char *key, char *decrypted_data) {
        return crypt(input, key, decrypted_data, false);
    }

    static bool crypt(const char * input, const char *key, char *result, bool enc_dec) {
        for(unsigned int i = 0; i < KEY_SIZE - 1; i++)
            result[i] = input[i] ^ key[i+1];
        result[KEY_SIZE - 1] = input[KEY_SIZE - 1] ^ key[0];
        return true;
    }
};

__END_SYS

#ifdef __CIPHER_H
#include __CIPHER_H
#else
__BEGIN_SYS
class Cipher: public IF<Traits<Xor_Cipher>::enabled, Xor_Cipher, Dummy>::Result {};
__END_SYS
#endif

#endif
