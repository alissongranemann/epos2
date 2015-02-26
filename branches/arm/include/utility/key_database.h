#ifndef _KEY_DATABASE_H
#define _KEY_DATABASE_H

#include <system/config.h>

__BEGIN_UTIL

template <typename Address>
class Key_Database
{
	const static unsigned int ID_SIZE = Traits<Secure_NIC>::ID_SIZE;
	const static unsigned int KEY_SIZE = Traits<Diffie_Hellman>::SECRET_SIZE;
	public:
	Key_Database();
	bool validate_peer(const char *sn, const char *ms, const char *auth, const Address a);
	bool insert_peer(const char *ms, const Address a);
	bool remove_peer(const char *ms, const Address a);
	bool insert_peer(const char *sn, const char *auth);
	bool remove_peer(const char *sn, const char *auth);

	bool auth_to_sn(char *serial_number, const char *auth, const Address addr);
//	bool sn_to_ms(char *master_secret, const char *serial_number, Address addr);
	bool addr_to_ms(char *master_secret, const Address addr);
	bool ms_to_sn(char *serial_number, const char *master_secret);

	private:
	typedef struct
	{
		char serial_number[ID_SIZE];
		char auth[16];
		bool free;
		bool validated;
	} Known_Node;

	typedef struct
	{
		char master_secret[KEY_SIZE];
		Address addr;
		Known_Node * node;
		bool free;
	} Authenticated_Peer;

	typedef struct
	{
		char master_secret[KEY_SIZE];
		Address addr;
		bool free;
	} Weak_Peer;

	Weak_Peer _weak_peers[16];
	Known_Node _known_nodes[16];
	Authenticated_Peer _peers[16];

	inline bool equals(const char *a, int sza, const char *b, int szb)
	{
		while((sza > 0) && ((a[sza-1] == 0) || (a[sza-1] == '0'))) sza--;
		while((szb > 0) && ((b[szb-1] == 0) || (b[szb-1] == '0'))) szb--;
		if(sza != szb) return false;
		for(int i=0;i<sza;i++)
			if(a[i] != b[i]) return false;
		return true;
	}
};

__END_UTIL
#endif
