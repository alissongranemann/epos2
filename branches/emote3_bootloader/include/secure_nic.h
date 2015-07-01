#ifndef __SECURE_NIC_H
#define __SECURE_NIC_H

#include <system/config.h>
#ifndef __no_networking__

#include <nic.h>
#include <ieee802_15_4.h>
#include <thread.h>
#include <cpu.h>
#include <utility/cipher.h>
#include <utility/observer.h>
#include <utility/key_database.h>
#include <poly1305.h>
#include <diffie_hellman.h>

__BEGIN_SYS;

class Secure_NIC : public Diffie_Hellman, public IEEE802_15_4::Observed, public IEEE802_15_4::Observer
{
public:        
    typedef IEEE802_15_4::Protocol Protocol;
    typedef IEEE802_15_4::Observed Observed;
    typedef IEEE802_15_4::Observer Observer;
    typedef IEEE802_15_4::Frame Frame;
    typedef IEEE802_15_4::Buffer Buffer;
    typedef IEEE802_15_4::Address Address;
    typedef Key_Database<Address> Key_Db;

    Address broadcast() { return _nic->broadcast(); }
    void free(Buffer *b){_nic->free(b);}

	const static unsigned int ID_SIZE = Traits<Build>::ID_SIZE;

	// Round up to a multiple of 16, for AES
    static unsigned int round16(const unsigned int n) { return n + 15 - ((n - 1) % 16); }

	Secure_NIC(bool is_gateway, Cipher *c, Poly1305 *p, NIC *nic = new NIC(), Key_Db *db = new Key_Db())
		: Diffie_Hellman()
	{
		_is_gateway = is_gateway;
        _authenticating = false;
		_nic = nic; _keydb = db; _cipher = c; _poly = p;

        if(_is_gateway)
            gateway_address = _nic->address();

		_nic->attach(this, Traits<Secure_NIC>::PROTOCOL_ID);

		_waiting_dh_response = false;
		_waiting_auth_response = false;
		_authenticated = false;
		accepting_connections = false;
	}

	// Waits for a secure message.
	// The message is delivered already decrypted.
	int receive(NIC::Address &from, char *data, unsigned int size)
	{
		while(!_has_received_data)
			Thread::yield();
		db<Secure_NIC>(INF) << "Secure_NIC Received data" << endl;
		from = _received_from;
		for(unsigned int i=0; i < _received_length && i < size; i++)
			data[i] = _received_data[i];
		_has_received_data = false;
		return _received_length;
	}

	// Used by the sensor.
	// Sets the ID and AUTH of this sensor.
	void set_id(const char _id[ID_SIZE])
	{
		for(unsigned int i=0;i<ID_SIZE;i++)
			_serial_number[i] = _id[i];
		calculate_auth(_auth, _serial_number);
	}
	void get_id(char _id[ID_SIZE])
	{
		for(unsigned int i=0;i<ID_SIZE;i++)
			_id[i] = _serial_number[i];
	}

	// Used by the server.
	// Inserts an ID that should be trusted.
	bool insert_trusted_id(const char *id)
	{
		calculate_auth(_auth, id);
		return _keydb->insert_peer(id, _auth);
	}
	// Used by the server.
	// This ID should not be trusted anymore.
	bool remove_trusted_id(const char *id)
	{ 
		calculate_auth(_auth, id);
		return _keydb->remove_peer(id, _auth);
	}

	// Used by the sensor.
	// Requests to start the protocol and establish a key.
	int send_key_request(NIC::Address dest);

	// Sends a secure message to an already authenticated destination.
	int send(const NIC::Address &dst, const char *data, unsigned int size);
	// Sends a secure message to an already authenticated destination.
	int send(const char * id, const char *data, unsigned int size);

	// Used by the sensor.
	// Am I authenticated yet?
	bool authenticated(){ return _authenticated; }

	// Used by the server.
	// Set to true to start accepting authentication requests.
	// Set to false to reject authentication requests.
	volatile bool accepting_connections;

	void update(Observed * o, Protocol p, Buffer *b);

	NIC::Address gateway_address;

	private:
	// Type of message
	enum 
	{
		USER_MSG,
		DH_REQUEST,
		DH_RESPONSE,
		AUTH_REQUEST,
		AUTH_OK,
	};

	// Calculate AUTH code for a given ID
	void calculate_auth(char out[16], const char id[ID_SIZE])
	{
		char tmp[16];
		char tmp2[16];
		unsigned int i;
		for(i=0; i<ID_SIZE && i<16; i++)
		{
			tmp[i] = id[i];
			tmp2[i] = id[i];
			out[i] = 0;
		}
		for(; i<16; i++)
		{
			tmp[i] = 0;
			tmp2[i] = 0;
			out[i] = 0;
		}
		_cipher->mode(Cipher::CBC);
		_cipher->encrypt(tmp, tmp2, out);
		_cipher->mode(Cipher::ECB);

		db<Secure_NIC>(TRC) << "Secure_NIC::calculate_auth"<< endl;
		db<Secure_NIC>(TRC) << "id = [";
		for(i=0;i<ID_SIZE;i++)
			db<Secure_NIC>(TRC) << ", " << (int)id[i];
		db<Secure_NIC>(TRC) << "]" << endl;
		db<Secure_NIC>(TRC) << "auth = [";
		for(i=0;i<16;i++)
			db<Secure_NIC>(TRC) << ", " << (int)out[i];
		db<Secure_NIC>(TRC) << "]" << endl;
	}

	// Critical sections control
    void _acquire_lock(volatile bool & lock) { while(CPU::tsl(lock)) Thread::yield(); }
    void _release_lock(volatile bool & lock) { lock = false; }
	volatile bool _update_lock; 

	// Internal functions
	NIC * _nic;
	Key_Db * _keydb;
	Cipher * _cipher;
	Poly1305 * _poly;
	volatile bool _waiting_dh_response;
	volatile bool _waiting_auth_response;
	volatile bool _authenticated;
	NIC::Address _waiting_dh_from;
	NIC::Address _waiting_auth_from;
	char _auth[16];
	char _serial_number[ID_SIZE];
	int _send_dh_response(NIC::Address dest);
	int _send_otp(NIC::Address dest);
	int _send(const NIC::Address &dst, char *data, unsigned int size) 
	{
		db<Secure_NIC>(INF) << "Secure_NIC: Sending msg of size " << size << ", header " << (int)data[0] << endl;
		int ret = _nic->send(dst, Traits<Secure_NIC>::PROTOCOL_ID, data, size); 
// 		int ret = _nic->send(_nic->broadcast(), Traits<Secure_NIC>::PROTOCOL_ID, data, size); 
		db<Secure_NIC>(TRC) << "Secure_NIC: nic::send returned " << ret << endl;
		return ret;
	}
	bool _encrypt(char *encrypted_msg, const char *msg, unsigned int size, NIC::Address dst);
	bool _decrypt(char *decrypted_msg, const char *msg, unsigned int size, NIC::Address from);
	bool _derive_key(char *key, char *ms, char *sn);
	bool _authenticate(const char *msg, NIC::Address from);

	bool _is_gateway;
    volatile bool _authenticating;

	NIC::Address _received_from;
	char _received_data[NIC::MTU];
	unsigned int _received_length;
	int _receive(NIC::Address &from, char *data, unsigned int size);
	bool _has_received_data;
};

__END_SYS;
#endif
#endif
