#include <utility/key_database.h>

__BEGIN_UTIL

template<typename Address>
Key_Database<Address>::Key_Database()
{
	for(int i=0;i<16;i++)
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
	for(int i=0;i<16;i++)
	{
		if(_known_nodes[i].free)
		{
			_known_nodes[i].free = false;
			for(unsigned int j=0;j<ID_SIZE;j++)			
				_known_nodes[i].serial_number[j] = sn[j];				
			for(int j=0;j<16;j++)
				_known_nodes[i].auth[j] = auth[j];			
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::remove_peer(const char *sn, const char *auth)
{
	for(int i=0;i<16;i++)
	{
		if(!_known_nodes[i].free && equals(_known_nodes[i].serial_number, ID_SIZE, sn, ID_SIZE))
		{
			_known_nodes[i].free = true;
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::insert_peer(const char *ms, const Address a)
{
	for(int i=0;i<16;i++)
	{
		if(_weak_peers[i].free)
		{
			_weak_peers[i].free = false;
			_weak_peers[i].addr = a;
			for(unsigned int j=0;j<KEY_SIZE;j++)
			{
				_weak_peers[i].master_secret[j] = ms[j];
			}
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::remove_peer(const char *ms, const Address a)
{
	for(int i=0;i<16;i++)
	{
		if(!_weak_peers[i].free && (a == _weak_peers[i].addr))
		{
			_weak_peers[i].free = true;
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::auth_to_sn(char *serial_number, const char *auth, const Address addr)
{
	for(int i=0;i<16;i++)
	{
		if(!_known_nodes[i].free && equals(_known_nodes[i].auth, 16, auth, 16))
		{
			for(unsigned int j=0;j<ID_SIZE;j++)
				serial_number[j] = _known_nodes[i].serial_number[j];
			_peers[i].addr = addr;
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::addr_to_ms(char *master_secret, const Address addr)
{
	for(unsigned int i=0;i<16;i++)
	{
		if(!_peers[i].free && (_peers[i].addr == addr))
		{
			for(unsigned int j=0;j<KEY_SIZE;j++)
				master_secret[j] = _peers[i].master_secret[j];
			return true;
		}
		if(!_weak_peers[i].free && (_weak_peers[i].addr == addr))
		{
			for(unsigned int j=0;j<KEY_SIZE;j++)
				master_secret[j] = _weak_peers[i].master_secret[j];
			return true;
		}
	}
	return false;
}

template<typename Address>
bool Key_Database<Address>::ms_to_sn(char *serial_number, const char *master_secret)
{
	for(unsigned int i=0;i<16;i++)
		if(!_peers[i].free && equals(_peers[i].master_secret, KEY_SIZE, master_secret, KEY_SIZE))
		{
			for(unsigned int j=0;j<ID_SIZE;j++)
				serial_number[j] = _peers[i].node->serial_number[j];
			return true;
		}
	return false;
}

template<typename Address>
bool Key_Database<Address>::validate_peer(const char *sn, const char *ms, const char *auth, const Address a)
{
	for(unsigned int i=0;i<16;i++)
	{
		if(!_weak_peers[i].free && (_weak_peers[i].addr == a) && (equals(_weak_peers[i].master_secret, KEY_SIZE, ms, KEY_SIZE)))
		{
			for(unsigned int j=0;j<16;j++)
			{
				if(!_known_nodes[j].free && !_known_nodes[j].validated && equals(_known_nodes[j].serial_number, ID_SIZE, sn, ID_SIZE))
				{
					for(unsigned int k=0;k<16;k++)
					{
						if(_peers[k].free)
						{
							_peers[k].free = false;
							_peers[k].addr = a;
							_peers[k].node = &_known_nodes[j];
							for(unsigned int l=0;l<KEY_SIZE;l++)
								_peers[k].master_secret[l] = ms[l];
							_weak_peers[i].free = true;
							_known_nodes[j].validated = true;
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
	for(int i=0;i<16;i++)
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
