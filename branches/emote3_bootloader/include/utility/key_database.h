#ifndef _KEY_DATABASE_H
#define _KEY_DATABASE_H

#include <system/config.h>

__BEGIN_UTIL

template <typename Address>
class Key_Database
{
	const static unsigned int ID_SIZE = Traits<Build>::ID_SIZE;
	const static unsigned int KEY_SIZE = Traits<Diffie_Hellman>::SECRET_SIZE;
	const static unsigned int N_ENTRIES = 32;
	public:
	Key_Database();
	bool validate_peer(const char *sn, const char *ms, const char *auth, const Address a);
	bool insert_peer(const char *ms, const Address a);
	bool remove_peer(const char *ms, const Address a);
	bool insert_peer(const char *sn, const char *auth);
	bool remove_peer(const char *sn, const char *auth);

	bool auth_to_sn(char *serial_number, const char *auth, const Address addr);
    bool sn_to_addr(const char* serial_number, Address & addr);
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

	Weak_Peer _weak_peers[N_ENTRIES];
	Known_Node _known_nodes[N_ENTRIES];
	Authenticated_Peer _peers[N_ENTRIES];

    void dump()
    {        
        for(auto i=0u;i<N_ENTRIES;i++)
            if(!_peers[i].free)
            {
                kout << "_peers["<<i<<"] = "<< endl << "[";
                for(auto j=0u;j<KEY_SIZE;j++)                
                    kout << (int)_peers[i].master_secret[j] << (j==KEY_SIZE-1?"]":",");
                kout << endl << _peers[i].addr
                     << endl << _peers[i].node << endl;
            }            
        for(auto i=0u;i<N_ENTRIES;i++)
            if(!_weak_peers[i].free)
            {
                kout << "_weak_peers["<<i<<"] = "<< endl << "[";
                for(auto j=0u;j<KEY_SIZE;j++)                
                    kout << (int)_weak_peers[i].master_secret[j] << (j==KEY_SIZE-1?"]":",");
                kout << endl << _weak_peers[i].addr << endl;
            }            
        for(auto i=0u;i<N_ENTRIES;i++)
            if(!_known_nodes[i].free && _known_nodes[i].validated)
            {
                kout << "_known_nodes["<<i<<"] = "<< &_known_nodes[i] << endl << "[";
                for(auto j=0u;j<ID_SIZE;j++)                
                    kout << (int)_known_nodes[i].serial_number[j] << (j==ID_SIZE-1?"]":",");
                kout << endl << "[";
                for(auto j=0u;j<16;j++)                
                    kout << (int)_known_nodes[i].auth[j] << (j==16-1?"]":",");
                kout << endl << _known_nodes[i].validated << endl;;
            }            
    }

	bool equals(const char *a, int sza, const char *b, int szb)
	{
		while((sza > 0) && ((a[sza-1] == 0) || (a[sza-1] == '0'))) sza--;
		while((szb > 0) && ((b[szb-1] == 0) || (b[szb-1] == '0'))) szb--;
		if(sza != szb) return false;
		for(int i=0;i<sza;i++)
			if(a[i] != b[i]) return false;
		return true;
	}
};

template<typename Address>
Key_Database<Address>::Key_Database()
{
	for(auto i=0u;i<N_ENTRIES;i++)
	{
		_peers[i].free = true;
		_weak_peers[i].free = true;
		_known_nodes[i].free = true;
		_known_nodes[i].validated = false;
	}
}

template<typename Address>
bool Key_Database<Address>::insert_peer(const char *sn, const char *auth)
{
    //kout << "Key_Database::insert_peer" << endl;
	for(auto i=0u;i<N_ENTRIES;i++)
	{
		if(_known_nodes[i].free)
		{
			_known_nodes[i].free = false;
			for(auto j=0u;j<ID_SIZE;j++)			
				_known_nodes[i].serial_number[j] = sn[j];				
			for(auto j=0u;j<16;j++)
				_known_nodes[i].auth[j] = auth[j];			
            //dump();
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::remove_peer(const char *sn, const char *auth)
{
    //kout << "Key_Database::remove_peer" << endl;
	for(auto i=0u;i<N_ENTRIES;i++)
	{
		if(!_known_nodes[i].free && equals(_known_nodes[i].serial_number, ID_SIZE, sn, ID_SIZE))
		{
			_known_nodes[i].free = true;
            //dump();
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::insert_peer(const char *ms, const Address a)
{
    //kout << "Key_Database::insert_peer" << endl;
	for(auto i=0u;i<N_ENTRIES;i++)
	{
		if(_weak_peers[i].free)
		{
			_weak_peers[i].free = false;
			_weak_peers[i].addr = a;
			for(auto j=0u;j<KEY_SIZE;j++)
			{
				_weak_peers[i].master_secret[j] = ms[j];
			}
            //dump();
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::remove_peer(const char *ms, const Address a)
{
    //kout << "Key_Database::remove_peer" << endl;
	for(auto i=0u;i<N_ENTRIES;i++)
	{
		if(!_weak_peers[i].free && (a == _weak_peers[i].addr))
		{
			_weak_peers[i].free = true;
            //dump();
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::sn_to_addr(const char* serial_number, Address & addr)
{
    //kout << "Key_Database::sn_to_addr" << endl;
	for(auto i=0u;i<N_ENTRIES;i++)
	{
		if(!_peers[i].free && equals(_peers[i].node->serial_number, ID_SIZE, serial_number, ID_SIZE))
		{
            addr = _peers[i].addr;
            //dump();
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::auth_to_sn(char *serial_number, const char *auth, const Address addr)
{
    //kout << "Key_Database::auth_to_sn" << endl;
	for(auto i=0u;i<N_ENTRIES;i++)
	{
		if(!_known_nodes[i].free && equals(_known_nodes[i].auth, 16, auth, 16))
		{
			for(auto j=0u;j<ID_SIZE;j++)
				serial_number[j] = _known_nodes[i].serial_number[j];
            //dump();
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::addr_to_ms(char *master_secret, const Address addr)
{
    //kout << "Key_Database::addr_to_ms" << endl;
	for(auto i=0u;i<N_ENTRIES;i++)
	{
		if(!_peers[i].free && (_peers[i].addr == addr))
		{
			for(auto j=0u;j<KEY_SIZE;j++)
				master_secret[j] = _peers[i].master_secret[j];
            //dump();
			return true;
		}
		if(!_weak_peers[i].free && (_weak_peers[i].addr == addr))
		{
			for(auto j=0u;j<KEY_SIZE;j++)
				master_secret[j] = _weak_peers[i].master_secret[j];
            //dump();
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::ms_to_sn(char *serial_number, const char *master_secret)
{
    //kout << "Key_Database::ms_to_sn" << endl;
	for(auto i=0u;i<N_ENTRIES;i++)
		if(!_peers[i].free && equals(_peers[i].master_secret, KEY_SIZE, master_secret, KEY_SIZE))
		{
			for(auto j=0u;j<ID_SIZE;j++)
				serial_number[j] = _peers[i].node->serial_number[j];
            //dump();
			return true;
		}
	return false;
}

template<typename Address>
bool Key_Database<Address>::validate_peer(const char *sn, const char *ms, const char *auth, const Address a)
{
    //kout << "Key_Database::validate_peer" << endl;
	for(auto i=0u;i<N_ENTRIES;i++)
	{
		if(!_weak_peers[i].free && (_weak_peers[i].addr == a) && (equals(_weak_peers[i].master_secret, KEY_SIZE, ms, KEY_SIZE)))
		{
            // Revoke any other authenticated node under the same ID
            if(Traits<Secure_NIC>::ALLOW_MULTIPLE_NODES_WITH_SAME_ID)
            {
                for(auto j=0u;j<N_ENTRIES;j++)
                {
                    if((!_peers[j].free) && equals(_peers[j].node->serial_number, ID_SIZE, sn, ID_SIZE))
                        _peers[j].free = true;
                }
            }
            for(auto j=0u;j<N_ENTRIES;j++)
            {
                if(!_known_nodes[j].free 
                        && ( Traits<Secure_NIC>::ALLOW_MULTIPLE_NODES_WITH_SAME_ID || !_known_nodes[j].validated )
                        && equals(_known_nodes[j].serial_number, ID_SIZE, sn, ID_SIZE))
                {
                    for(auto k=0u;k<N_ENTRIES;k++)
                    {
                        if(_peers[k].free)
                        {
                            _peers[k].free = false;
                            _peers[k].addr = a;
                            _peers[k].node = &_known_nodes[j];
                            for(auto l=0u;l<KEY_SIZE;l++)
                                _peers[k].master_secret[l] = ms[l];
                            _weak_peers[i].free = true;
                            _known_nodes[j].validated = true;
                            //dump();
                            return true;
                        }
                    }
                }
            }
		}
	}
	return false;
}
/*
template<typename Address>
bool Key_Database<Address>::sn_to_ms(char *master_secret, const char *serial_number, Address addr)
{
	for(int i=0;i<N_ENTRIES;i++)
		if(!_peers[i].free && (_peers[i].addr == addr) && !strcmp(_peers[i].node->serial_number, serial_number))
		{
			_peers[i].addr = addr;
			master_secret = _peers[i].master_secret;
			return true;
		}
	return false;
}
*/
__END_UTIL
#endif
