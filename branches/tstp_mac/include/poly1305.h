#ifndef __poly1305_h
#define __poly1305_h

#include <utility/string.h>
#include <utility/bignum.h>
#include <utility/cipher.h>

#include <system/config.h>

__BEGIN_SYS

class Poly1305 : private Bignum
{
	public:

	Poly1305(char __k[16], char __r[16], Cipher *c);
	Poly1305(char __k[16], char __r[16]);
	Poly1305(Cipher *c);
	Poly1305();
	~Poly1305();

	void poly1305(char *out, const char *r, const char *s, const char *m, unsigned int l);

	void poly1305_bignum(char *out, const char *s, const char *m, unsigned int l);

	void authenticate( char out[16], const char n[16], const char * m, unsigned int l);

	bool verify(const char a[16], const char n[16], const char m[],unsigned int l);

	static bool isequal(const char x[16], const char y[16]);

	void aes(char outflip[16], const char n[16])
	{
		_cipher->encrypt(n, (char*)_k, outflip); 
        /*
		char kflip[16];
		char nflip[16];
		char out[16];
		int i;
        
		for(i=0;i<4;i++)
		{
			kflip[i] = _k[3-i];
			kflip[4+i] = _k[7-i];
			kflip[8+i] = _k[11-i];
			kflip[12+i] = _k[15-i];

			nflip[i] = n[3-i];
			nflip[4+i] = n[7-i];
			nflip[8+i] = n[11-i];
			nflip[12+i] = n[15-i];
		}        
		_cipher->encrypt(nflip, kflip, out); 
		for(i=0;i<4;i++)
		{
			outflip[i] = out[3-i];
			outflip[4+i] = out[7-i];
			outflip[8+i] = out[11-i];
			outflip[12+i] = out[15-i];
		}
        */
	}

	bool cipher(Cipher *c);
	void k(char k1[16]);
	void r(char r1[16]);
	void kr(char kr1[32]);

	const static unsigned int word = (17 + (sz_digit - 1)) / sz_digit;
	const static unsigned int sz_word = word * sz_digit;

	unsigned char _k[sz_word];
	unsigned char _r[sz_word];
private:
	Cipher * _cipher;

	// 2^(130) - 5
	const static unsigned char p1305_data[17];// = { 251,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,3 };
	const static digit default_barrett_u[word + 1];// = { 251,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,3 };
	unsigned char p1305[sz_word];
	char _mac[32];
};

__END_SYS
#endif
