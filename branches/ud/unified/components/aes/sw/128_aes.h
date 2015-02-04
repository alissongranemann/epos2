#ifndef AES_128
#define AES_128

	class aesKey {
	    public:
	        enum{
                CIPHER_SIZE   = 16
             };
        
        protected:
	        enum{
    	        EXPANDED_SIZE = 176,
                NK            = 4,
                NB            = 4,
                NR            = 10, // 128-bit AES
                TABLESIZE     = 256
             };
        
            static const unsigned char sBox[256];
            static const unsigned char inv_sBox[256];
            static const unsigned char Rcon[11];
	
		private:
		    void expandKey(unsigned char* key_in);
		public:
		    unsigned char key_schedule[EXPANDED_SIZE];
			unsigned char gf2(unsigned char value);	
			void addRoundKey(unsigned char* state, unsigned char* cipherKey, unsigned char round);
		    void changeKey(unsigned char* new_key);
	};

	class aesCipher: public aesKey {
		private:
			void mixColumns(unsigned char* in);
			void shiftRows(unsigned char* in);
			void subBytes(unsigned char* in);
			void cipher(unsigned char in[4*NB], unsigned char* key_in);
		public:
			void encrypt(unsigned char* plain_text);
	};

	class aesInvCipher: public aesKey {
	private:
		void invSubBytes(unsigned char* in);
		void invShiftRows(unsigned char* in);
		void invMixColumns(unsigned char* state);
		void invCipher(unsigned char in[4*NB], unsigned char* key_in);
	public:
		void decrypt(unsigned char* cipher_text);
	};

#endif
