#ifndef __CIPHER_H
#define __CIPHER_H

__BEGIN_UTIL;

class Cipher
{
	public:
	enum Mode
	{
		CBC,
		ECB,
		CTR,
		CTRMAC,
	};
	virtual bool encrypt(const char *data, const char *key, char *encrypted_data) = 0;
	virtual bool decrypt(const char *data, const char *key, char *decrypted_data) = 0;
	virtual bool mode(Mode m) { _mode = m; return true; };
	virtual Mode mode() { return _mode; };
	protected:
	Mode _mode;
};

__END_UTIL;

#endif
