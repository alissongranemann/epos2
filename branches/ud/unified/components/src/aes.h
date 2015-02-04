#ifndef __aes_common_h
#define __aes_common_h

namespace Implementation {

class AES_Common
{
protected:
    enum {
        EXPANDED_SIZE   = 176,
        CIPHER_SIZE     = 16,
        NK              = 4,
        NB              = 4,
        NR              = 10
    };

protected:
    unsigned char _expandKey(unsigned char * in);
    unsigned char * _cipher(unsigned char in[4*NB], unsigned char * key_schedule);
    unsigned char * _invCipher(unsigned char in[4*NB], unsigned char * key_schedule);

private:
    static const unsigned char _sBox[256];
    static const unsigned char _inv_sBox[256];
    static const unsigned char _Rcon[256];

private:
    void _rotate(unsigned char * in);
    void _scheduleCore(unsigned char * in, unsigned char i);

    void _mixColumns(unsigned char * in);
    void _subBytes(unsigned char * in);
    void _shiftRows(unsigned char * in);
    void _addRoundKey(unsigned char * state, unsigned char * cipherKey, unsigned char round);

    void _invMixColumns(unsigned char * in);
    void _invSubBytes(unsigned char * in);
    void _invShiftRows(unsigned char * in);
};

};

#endif
