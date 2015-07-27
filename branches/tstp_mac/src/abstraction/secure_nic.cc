#include <system/config.h>
#ifndef __no_networking__

#include <secure_nic.h>
//#include <clock.h>
// #include <tsc.h>
#include <utility/bignum.h>

__USING_SYS;

// Sends a secure message to an already authenticated destination.
int Secure_NIC::send(const char * id, const char *data, unsigned int size)
{
    while(_is_gateway && _authenticating);

    NIC::Address dst;
    if(!_keydb->sn_to_addr(id, dst))
        return -1;

	unsigned int sz = round16(size);

	// Header + (size rounded up to multiple of 16)
	char msg[1+sz];

	// Message header
	msg[0] = USER_MSG;

	// Copy user data and zero-pad
	unsigned int i;
	for(i=1;i<=size;i++) msg[i] = data[i-1];
	for(;i<sz;i++) msg[i]=0;

	// Encrypt the message
	if(!_encrypt(msg+1, msg+1, sz, dst))
		return -3;

	// Give the message to lower layer
	return _send(dst, msg, 1+sz);
}

// Sends a secure message to an already authenticated destination.
int Secure_NIC::send(const NIC::Address &dst, const char *data, unsigned int size)
{
    while(_is_gateway && _authenticating);

	unsigned int sz = round16(size);

	// Header + (size rounded up to multiple of 16)
	char msg[1+sz];

	// Message header
	msg[0] = USER_MSG;

	// Copy user data and zero-pad
	unsigned int i;
	for(i=1;i<=size;i++) msg[i] = data[i-1];
	for(;i<=sz;i++) msg[i]=0;

	// Encrypt the message
	if(!_encrypt(msg+1, msg+1, sz, dst))
		return -3;

	// Give the message to lower layer
	return _send(dst, msg, 1+sz);
}

// Used by the sensor.
// Requests to start the protocol and establish a key.
// Sends the public Diffie-Hellman key.
int Secure_NIC::send_key_request(NIC::Address dest)
{
	// Header + public key
	char msg[1 + PUBLIC_KEY_SIZE];
	msg[0] = DH_REQUEST;

	// Fetch the public Diffie-Hellman key
	get_public((unsigned char *)(msg+1), PUBLIC_KEY_SIZE);

	_waiting_dh_response = true;
	_waiting_dh_from = dest;
	
	// Give the message to lower layer
	return _send(dest, msg, 1+PUBLIC_KEY_SIZE);
}

// Response to DH_REQUEST
// Sends the public Diffie-Hellman key
int Secure_NIC::_send_dh_response(NIC::Address dest)
{
	// Header + public key
	char msg[1 + PUBLIC_KEY_SIZE];
	msg[0] = DH_RESPONSE;
	
	// Fetch the public Diffie-Hellman key
	get_public((unsigned char *)(msg+1), PUBLIC_KEY_SIZE);

	// Give the message to lower layer
	return _send(dest, msg, 1+PUBLIC_KEY_SIZE);
}

// Calculates and sends AUTH + OTP
int Secure_NIC::_send_otp(NIC::Address dest)
{
	//Header + AUTH + OTP
	char msg[1 + 16 + 16];	
	msg[0] = AUTH_REQUEST;

	// Fetch Master Secret
	if(!_keydb->addr_to_ms(msg+1, dest))
		return -1;

	// Calculate and attach OTP
	if(!_derive_key(msg+17, msg+1, _serial_number))
		return -2;

	// Attach AUTH
	for(int i=0;i<16;i++)
		msg[i+1] = _auth[i];

	// Give the message to lower layer
	return _send(dest, msg, 1+16+16);
}

// Derive the current key and use it to encrypt the message
bool Secure_NIC::_encrypt(char *encrypted_msg, const char *msg, unsigned int size, NIC::Address dst)
{
	// Master Secret
	char ms[SECRET_SIZE];
	// Current Key
	char key[16];

	// Fetch master secret
	if(!_keydb->addr_to_ms(ms,dst))
    {
        db<Secure_NIC>(ERR) << "Secure_NIC::_encrypt: could not find master secret from address" << endl;
		return false;
    }

    db<Secure_NIC>(TRC) << "Secure_NIC::_encrypt: Found master secret: [" << (int)ms[0];
    for(unsigned int i=1;i<SECRET_SIZE;i++)
        db<Secure_NIC>(TRC) << " ," << (int)ms[i];
    db<Secure_NIC>(TRC) << "]" << endl;

	if(_is_gateway)
	{
		// Fetch the ID associated to that master secret
		char sn[ID_SIZE];
		if(!_keydb->ms_to_sn(sn,ms))
        {
            db<Secure_NIC>(ERR) << "Secure_NIC::_encrypt: could not find serial number from master secret" << endl;
			return false;
        }

        db<Secure_NIC>(TRC) << "Secure_NIC::_encrypt: Found serial number: [" << (int)sn[0];
        for(unsigned int i=1;i<ID_SIZE;i++)
            db<Secure_NIC>(TRC) << " ," << (int)sn[i];
        db<Secure_NIC>(TRC) << "]" << endl;

		// Calculate the current key
		if(!_derive_key(key, ms, sn))
        {
            db<Secure_NIC>(ERR) << "Secure_NIC::_encrypt: could not derive key" << endl;;
			return false;
        }
	}
	// Calculate the current key
	else if(!_derive_key(key, ms, _serial_number))
    {
        db<Secure_NIC>(ERR) << "Secure_NIC::_encrypt: could not derive key" << endl;;
        return false;
    }

    /*
	   kout << "ms = [";
	   for(int i=0;i<SECRET_SIZE;i++)
	   kout << " ," << (int)ms[i];
	   kout << "]" << endl;
	   kout << "key = [";
	   for(int i=0;i<16;i++)
	   kout << " ," << (int)key[i];
	   kout << "]" << endl;
	   kout << "msg = [";
	   for(auto i=0u;i<size;i++)
	   kout << " ," << (int)msg[i];
	   kout << "]" << endl;
    */

	// Encrypt the message, 16 bytes at a time
	for(unsigned int i=0;i<size;i+=16)
	{
        char padded_msg[16];
        const char * tmp;
        if(i+16 <= size)
            tmp = msg+i;
        else
        {
            for(unsigned int j=0;j<16;j++)
            {
                if(j+i < size)
                    padded_msg[j] = msg[j+i];
                else
                    padded_msg[j] = 0;
            }
            tmp = (const char *)padded_msg;
        }

		if(!_cipher->encrypt(tmp, key, encrypted_msg+i))
        {
            db<Secure_NIC>(ERR) << "Secure_NIC::_encrypt: cipher error" << endl;
			return false;
        }
	}

    db<Secure_NIC>(TRC) << "Secure_NIC::_encrypt: encrypted msg: [" << (int)encrypted_msg[0];
    for(unsigned int i=1;i<round16(size);i++)
        db<Secure_NIC>(TRC) << " ," << (int)encrypted_msg[i];
    db<Secure_NIC>(TRC) << "]" << endl;

	return true;
}

// Derive the current key and use it to decrypt the message
bool Secure_NIC::_decrypt(char *decrypted_msg, const char *msg, unsigned int size, NIC::Address from)
{
	// Master Secret
	char ms[SECRET_SIZE];
	// Current Key
	char key[16];

	// Fetch master secret
	if(!_keydb->addr_to_ms(ms,from))
    {
        db<Secure_NIC>(ERR) << "Secure_NIC::_decrypt: Couldn't get master secret from address" << endl;
		return false;
    }
	
	if(_is_gateway)
	{
		char sn[ID_SIZE];
		// Fetch the ID associated to that master secret
		if(!_keydb->ms_to_sn(sn,ms))
        {
            db<Secure_NIC>(ERR) << "Secure_NIC::_decrypt: Couldn't get serial number from master secret" << endl;
			return false;
        }
		// Calculate the current key
		if(!_derive_key(key, ms, sn))
        {
            db<Secure_NIC>(ERR) << "Secure_NIC::_decrypt: Couldn't derive key" << endl;
			return false;
        }
	}
	// Calculate the current key
	else if(!_derive_key(key, ms, _serial_number))
    {
        db<Secure_NIC>(ERR) << "Secure_NIC::_decrypt: Couldn't derive key" << endl;
        return false;
    }

	// Decrypt the message, 16 bytes at a time
	for(unsigned int i=0;i<size;i+=16)
	{
        char padded_msg[16];
        const char * tmp;
        if(i+16 <= size)
            tmp = msg+i;
        else
        {
            for(unsigned int j=0;j<16;j++)
            {
                if(j+i < size)
                    padded_msg[j] = msg[j+i];
                else
                    padded_msg[j] = 0;
            }
            tmp = (const char *)padded_msg;
        }

		if(!_cipher->decrypt(tmp, key, decrypted_msg+i))
        {
            db<Secure_NIC>(ERR) << "Secure_NIC::_decrypt: cipher error" << endl;
			return false;
        }
	}
    db<Secure_NIC>(TRC) << "Secure_NIC::_decrypt: decrypted msg: [" << (int)decrypted_msg[0];
    for(unsigned int i=1;i<round16(size);i++)
        db<Secure_NIC>(TRC) << " ," << (int)decrypted_msg[i];
    db<Secure_NIC>(TRC) << "]" << endl;
	return true;
}

// Calculate OTP
bool Secure_NIC::_derive_key(char *key, char *ms, char *sn)
{
	char _sn[16];
	char _ms[16];
	unsigned int i;
	for(i=0;i<ID_SIZE && i<16;i++)
		_sn[i] = sn[i];
	for(;i<16;i++)
		_sn[i] = 0;
	for(i=0;i<SECRET_SIZE && i<16;i++)
		_ms[i] = ms[i];
	for(;i<16;i++)
		_ms[i] = 0;

	// Calculating OTP
	_poly->r(_ms);
	_poly->k(_sn);
		
// 	union
// 	{
		char time[16];
// 		unsigned long long s;
// 	};

	for(int i=0;i<16;i++)
		time[i] = 0;

    /*
	s = TSC::getMicroseconds();
	// Round up to a multiple of the time window
	s = s + Traits<Secure_NIC>::TIME_WINDOW - 1 - (s - 1) % Traits<Secure_NIC>::TIME_WINDOW;

	kout << "s = " << s << endl;
    */
	const unsigned int maxsz = ID_SIZE > SECRET_SIZE ? ID_SIZE : SECRET_SIZE;
	char m[maxsz];
	for(unsigned int i=0; i<maxsz; i++)
		m[i] = _ms[i % SECRET_SIZE] ^ _sn[i % ID_SIZE];

    db<Secure_NIC>(TRC) << "Secure_NIC::_derive_key: serial number: [" << (int)_sn[0];
    for(unsigned int i=1;i<ID_SIZE;i++)
        db<Secure_NIC>(TRC) << " ," << (int)_sn[i];
    db<Secure_NIC>(TRC) << "]" << endl;
    db<Secure_NIC>(TRC) << "Secure_NIC::_derive_key: master secret: [" << (int)_ms[0];
    for(unsigned int i=1;i<SECRET_SIZE;i++)
        db<Secure_NIC>(TRC) << " ," << (int)_ms[i];
    db<Secure_NIC>(TRC) << "]" << endl;
    db<Secure_NIC>(TRC) << "Secure_NIC::_derive_key: m: [" << (int)m[0];
    for(unsigned int i=1;i<maxsz;i++)
        db<Secure_NIC>(TRC) << " ," << (int)m[i];
    db<Secure_NIC>(TRC) << "]" << endl;
    db<Secure_NIC>(TRC) << "Secure_NIC::_derive_key: time: [" << (int)time[0];
    for(unsigned int i=1;i<16;i++)
        db<Secure_NIC>(TRC) << " ," << (int)time[i];
    db<Secure_NIC>(TRC) << "]" << endl;

	_poly->authenticate(key, time, m, maxsz);

    db<Secure_NIC>(TRC) << "Secure_NIC::_derive_key: derived key: [" << (int)key[0];
    for(unsigned int i=1;i<16;i++)
        db<Secure_NIC>(TRC) << " ," << (int)key[i];
    db<Secure_NIC>(TRC) << "]" << endl;

	return true;
}

// signed_msg = AUTH + OTP.
// Calculate the OTP and check if it matches the one in the msg
bool Secure_NIC::_authenticate(const char *signed_msg, NIC::Address dst)
{
	db<Secure_NIC>(TRC) << "Secure_NIC::_authenticate()" << endl;
	if(!accepting_connections) 
	{
		db<Secure_NIC>(WRN) << "Secure_NIC::_authenticate() - not accepting connections" << endl;
		return false;
	}

	char sn[ID_SIZE];
	char ms[SECRET_SIZE];
	char otp[16];

	// Fetch the ID associated to this AUTH
	if(!_keydb->auth_to_sn(sn, signed_msg, dst))
	{
		db<Secure_NIC>(ERR) << "Secure_NIC::_authenticate() - failed to get sn" << endl;
		return false;
	}

	// Fetch the address associated to this master secret
	if(!_keydb->addr_to_ms(ms, dst))
	{
		db<Secure_NIC>(ERR) << "Secure_NIC::_authenticate() - failed to get ms" << endl;
		return false;
	}

	// Calculate OTP
	if(!_derive_key(otp, ms, sn))
	{
		db<Secure_NIC>(ERR) << "Secure_NIC::_authenticate() - failed to get otp" << endl;
		return false;
	}

	db<Secure_NIC>(INF) << "Secure_NIC::authenticating..." << endl;

	// Check if calculated OTP matches the received one
	if( _poly->isequal(signed_msg+16, otp) )
	{
		 db<Secure_NIC>(INF)<< "Secure_NIC::authentication granted" << endl;
		_keydb->validate_peer(sn, ms, signed_msg, dst);

		// Tell the sensor that it is authenticated
// 		char msg[1 + ID_SIZE];
 		char msg[1 + round16(ID_SIZE)];
		if(!_encrypt(msg+1, sn, ID_SIZE, dst))
		{
			db<Secure_NIC>(ERR)<< "Secure_NIC - encryption failed" << endl;
			return false;
		}
		msg[0] = AUTH_OK;

// 		_send(dst, msg, 1 + ID_SIZE);
 		_send(dst, msg, 1 + round16(ID_SIZE));

		return true;
	}
	else
	{
		db<Secure_NIC>(ERR) << "Secure_NIC::authentication failed" << endl;
		_keydb->remove_peer(ms, dst);
		return false;
	}
}

void Secure_NIC::update(Observed * o, Protocol p, Buffer * b)
{
    unsigned int i;
    char tmp[round16(ID_SIZE)];
    unsigned char aux[SECRET_SIZE];
    bool freed = false;
    int x;
// 	_acquire_lock(_update_lock);
	db<Secure_NIC>(TRC) << "Secure_NIC::update()" << endl;
	if(p == Traits<Secure_NIC>::PROTOCOL_ID)
	{
        Frame *frame = b->frame();
        char *data = frame->data<char>();
        unsigned int size = b->size();
        auto src = frame->src();
 		db<Secure_NIC>(INF) << "Secure_NIC - Received msg of size " << size << ", header " << (int)data[0] << endl;
		
		switch(data[0])
		{
			case USER_MSG:
				// Decrypt the message
				if(!_decrypt(data, data+1, size-1, src))
					break;
                db<Secure_NIC>(TRC) << "Notifying upper layer" << endl;
                b->size(size-1);
				if(!notify(p,b))
                {
                    freed = true;
                    _nic->free(b);
                }
				break;
			case DH_REQUEST:
				if(!accepting_connections || size != PUBLIC_KEY_SIZE + 1) 
					break;
                if(_is_gateway)
                    _authenticating = true;
				// Send back Diffie_Hellman public key
				_send_dh_response(src);
				// Set a Diffie-Hellman Master Secret
				calculate_key(aux, SECRET_SIZE, (unsigned char *)(data+1), PUBLIC_KEY_SIZE);
				_keydb->insert_peer((char *)aux, src);
				break;
			case DH_RESPONSE:
				if(!_waiting_dh_response || size != PUBLIC_KEY_SIZE + 1) 
					break;
				if(!((_waiting_dh_from == IEEE802_15_4::broadcast()) || (_waiting_dh_from == src))) 
					break;
				_waiting_dh_response = false;
				// Set a Diffie-Hellman Master Secret
				calculate_key(aux, SECRET_SIZE, (unsigned char *)(data+1), PUBLIC_KEY_SIZE);
				_keydb->insert_peer((char *)aux, src);
				_waiting_auth_response = true;
				_waiting_auth_from = src;
				// Send authentication message
                x = _send_otp(src);
				db<Secure_NIC>(TRC) << "_send_otp returned " << x << endl;
				break;
			case AUTH_REQUEST:
                if(_is_gateway)
                    _authenticating = false;
				if(size < 33) 
					break;
				_authenticate(data+1, src);
				break;
			case AUTH_OK:
// 				if(!_waiting_auth_response || size != 1+ID_SIZE)
 				if(!_waiting_auth_response || size != 1+round16(ID_SIZE))
				{
					db<Secure_NIC>(ERR) << "Secure_NIC - unexpected msg" << endl;
					break;
				}
				if(!(_waiting_auth_from == src)) 
				{
					db<Secure_NIC>(ERR) << "Secure_NIC - address mismatch" << endl;
					break;
				}
				
// 				if(!_decrypt(tmp, (const char *)(data+1), ID_SIZE, src))
				if(!_decrypt(tmp, (const char *)(data+1), round16(ID_SIZE), src))
				{
					db<Secure_NIC>(ERR) << "Secure_NIC - decryption failed" << endl;
					break;
				}
				for(i=0; i<ID_SIZE; i++)
					if(tmp[i] != _serial_number[i])
						break;
				if(i<ID_SIZE) 
				{
					db<Secure_NIC>(ERR) << "Secure_NIC - AUTH_OK msg wrong" << endl;
                    for(i=0;i<ID_SIZE;i++)
                        db<Secure_NIC>(INF) << (int)tmp[i] << " / " << (int)_serial_number[i] << endl;
					break;
				}
				_waiting_auth_response = false;
				_authenticated = true;
                gateway_address = src;
				// Sensor is authenticated
				db<Secure_NIC>(INF) << "Secure_NIC - Authenticated" << endl;
				break;
			default:
				break;
		}
        if(!freed)
            _nic->free(b);
	}
// 	_release_lock(_update_lock);
}
#endif
