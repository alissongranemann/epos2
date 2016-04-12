#include <system/config.h>

#include <ether_channel.h>

__BEGIN_SYS

int Ether_Channel::receive(NIC::Buffer * buf, void * d, unsigned int s)
{
    /// TODO
#if 0
    unsigned char * data = reinterpret_cast<unsigned char *>(d);

    memcpy(data, buf->frame()->data<void>(), len);

    return size;
#endif
    return 0;
}

void Ether_Channel::update(NIC::Observed * obs, NIC::Protocol prot, NIC::Buffer * pool)
{
    /// TODO
#if 0
    db<Ether_Channel>(TRC) << "Ether_Channel::update(obs=" << obs << ",prot=" << prot << ",buf=" << pool << ")" << endl;

    Packet * packet = pool->frame()->data<Packet>();
    Message * message = packet->data<Message>();

    db<Ether_Channel>(INF) << "Ether_Channel::update:msg=" << message << " => " << *message << endl;

    if(!notify(message->to(), pool))
        pool->nic()->free(pool);

   ////////
   cout << "  Data: " << (char *) pool->frame()->data<void>() << endl;
#endif
}

__END_SYS
