// EPOS Communicator Declarations

#ifndef __communicator_h
#define __communicator_h

#include <channel.h>

__BEGIN_SYS 

// Commonalities for connectionless channels
template<typename Channel, bool connectionless>
class Communicator_Common: protected Channel::Observer
{
private:
    static const unsigned int HEADERS_SIZE = Channel::HEADERS_SIZE;

public:
    // List to hold received Buffers
    typedef typename Channel::Buffer Buffer;
    typedef typename Buffer::List List;
    typedef typename List::Element Element;

    // Addresses
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;
    typedef typename Channel::Observer::Observing_Condition Observing_Condition;

protected:
    Communicator_Common(const Local_Address & local): _local(local), _ready(0) {
        Channel::attach(this, local);
    }

public:
    ~Communicator_Common() {
        Channel::detach(this, _local);
    }

    int send(const Address & to, const void * data, unsigned int size) {
        return Channel::send(_local, to, data, size);
    }
    int send(const Local_Address & from, const Address & to, const void * data, unsigned int size) {
        return Channel::send(from, to, data, size);
    }

    int receive(void * data, unsigned int size) {
        _ready.p();
        Element * el = _received.remove();
        Buffer * buf = el->object();
        return Channel::receive(buf, data, size);
    }
    int receive(Address * from, void * data, unsigned int size) {
        _ready.p();
        Element * el = _received.remove();
        Buffer * buf = el->object();
        return Channel::receive(buf, from, data, size);
    }

    int receive_all(void * data, unsigned int size) {
        int r = 0;
        for(unsigned int received = 0, coppied = 0; received < size; received += coppied) {
            _ready.p();
            Element * e = _received.remove();
            Buffer * head = e->object();
            r += Channel::receive(head, data + received, coppied = ((received + (head->size() - HEADERS_SIZE)) > size ? (size - received) : (head->size() - HEADERS_SIZE)));
        }

        return r;
    }
    int receive_all(Address * from, void * data, unsigned int size) {
        int r = 0;
        for(unsigned int received = 0, coppied = 0; received < size; received += coppied) {
            _ready.p();
            Element * e = _received.remove();
            Buffer * head = e->object();
            r += Channel::receive(head, data + received, coppied = ((received + (head->size() - HEADERS_SIZE)) > size ? (size - received) : (head->size() - HEADERS_SIZE)));
        }

        return r;
    }

private:
    void update(typename Channel::Observed * obs, Observing_Condition c, Buffer * buf) {
        _received.insert(buf->lext());
        _ready.v();
    }

protected:
    Local_Address _local;
    Semaphore _ready;
    List _received;
};

// Commonalities for connection-oriented channels
template<typename Channel>
class Communicator_Common<Channel, false>: protected Channel::Observer
{
private:
    static const unsigned int HEADERS_SIZE = Channel::HEADERS_SIZE;

public:
    // List to hold received Buffers
    typedef typename Channel::Buffer Buffer;
    typedef typename Buffer::List List;
    typedef typename List::Element Element;

    // Addresses
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;
    typedef typename Channel::Observer::Observing_Condition Observing_Condition;

protected:
    Communicator_Common(const Local_Address & local, const Address & peer): _local(local), _ready(0) {
        _connection = Channel::attach(this, local, peer);
    }

public:
    ~Communicator_Common() {
        Channel::detach(this, _connection);
    }

    int send(const void * data, unsigned int size) {
        return _connection->send(data, size);
    }

    int receive_some(void * data, unsigned int size) {
        _ready.p();
        Element * el = _received.remove();
        Buffer * buf = el->object();
        return _connection->receive(buf, data, size);
    }

    int receive(void * d, unsigned int size) {
        char * data = reinterpret_cast<char *>(d);
        unsigned int received = 0;
        do {
            _ready.p();
            Element * el = _received.remove();
            Buffer * buf = el->object();
            unsigned int segment_size = _connection->receive(buf, data, size);
            data += segment_size;
            received += segment_size;
        } while(received <= size);
        return size;
    }

    int receive_all(void * d, unsigned int size) {
        char * data = reinterpret_cast<char *>(d);
        int r = 0;
        for(unsigned int received = 0, coppied = 0; received < size; received += coppied) {
            _ready.p();
            Element * e = _received.remove();
            Buffer * head = e->object();
            r += _connection->receive(head, data + received, coppied = ((received + (head->size() - HEADERS_SIZE)) > size ? (size - received) : (head->size() - HEADERS_SIZE)));
        }

        return r;
    }

private:
    void update(typename Channel::Observed * obs, Observing_Condition c, Buffer * buf) {
        _received.insert(buf->lext());
        _ready.v();
    }

protected:
    Local_Address _local;
    Semaphore _ready;
    List _received;

    typename Channel::Connection * _connection;
};


// Link (point-to-point communicator) connectionless channels
template<typename Channel, bool connectionless = Channel::connectionless>
class Link: public Communicator_Common<Channel, connectionless>
{
private:
    typedef Communicator_Common<Channel, connectionless> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Link(const Local_Address & local, const Address & peer = Address::NULL): Base(local), _peer(peer) {}
    ~Link() {}

    int send(const void * data, unsigned int size) { return Base::send(_peer, data, size); }
    int receive(void * data, unsigned int size) { return Base::receive(data, size); }
    int receive_all(void * data, unsigned int size) { return Base::receive_all(data, size); }

    int read(void * data, unsigned int size) { return receive_all(data, size); }
    int write(const void * data, unsigned int size) { return send(data, size); }

    const Address & peer() const { return _peer;}

private:
    Address _peer;
};

// Link (point-to-point communicator) for connection-oriented channels
template<typename Channel>
class Link<Channel, false>: public Communicator_Common<Channel, false>
{
private:
    typedef Communicator_Common<Channel, false> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Link(const Local_Address & local, const Address & peer = Address::NULL): Base(local, peer), _peer(peer) {}
    ~Link() {}

    int send(const void * data, unsigned int size) { return Base::send(data, size); }
    int receive(void * data, unsigned int size) { return Base::receive(data, size); }
    int receive_all(void * data, unsigned int size) { return Base::receive_all(data, size); }

    int read(void * data, unsigned int size) { return receive_all(data, size); }
    int write(const void * data, unsigned int size) { return send(data, size); }

    const Address & peer() const { return _peer;}

private:
    Address _peer;
};


// Port (1-to-N communicator) for connectionless channels
template<typename Channel, bool connectionless = Channel::connectionless>
class Port: public Communicator_Common<Channel, connectionless>
{
private:
    typedef Communicator_Common<Channel, connectionless> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Port(const Local_Address & local): Base(local) {}
    ~Port() {}

    int send(const Address & to, const void * data, unsigned int size) { return Base::send(to, data, size); }
    int receive(Address * from, void * data, unsigned int size) { return Base::receive(from, data, size); }
};

// Port (1-to-N communicator) for connection-oriented channels
template<typename Channel>
class Port<Channel, false>: public Communicator_Common<Channel, false>
{
private:
    typedef Communicator_Common<Channel, false> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Port(const Local_Address & local): Base(local) {}
    ~Port() {}

    Link<Channel> * listen() { return new Link<Channel>(Channel::listen(this->_local)); }
    Link<Channel> * connect(const Address & to) { return new Link<Channel>(Channel::connect(this->_local, to)); }
};

__END_SYS

#endif
