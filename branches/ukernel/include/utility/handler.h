// EPOS Handler Utility Declarations

#ifndef __handler_h
#define __handler_h

#include <system/config.h>
#include <utility/list.h>

__BEGIN_UTIL

class Handler
{
public:
    // A handler function
    typedef void (Function)();

    typedef Simple_List<Handler>::Element Element;

public:
    Handler():  _link(this)
    {
    }

    virtual ~Handler() {}

    virtual void operator()() = 0;


    Element * link() { return &_link; }

protected:
    Element _link;
};

class Dual_Handler: public Handler
{
public:
    virtual void dual() = 0;
};

class Function_Handler: public Handler
{
public:
    Function_Handler(Function * h): _handler(h) {}
    ~Function_Handler() {}

    void operator()() { _handler(); }

private:
    Function * _handler;
};

template<typename T>
class Functor_Handler: public Handler
{
public:
    typedef void (Functor)(T *);

    Functor_Handler(Functor * h, T * p): _handler(h), _ptr(p) {}
    ~Functor_Handler() {}

    void operator()() { _handler(_ptr); }

private:
    Functor * _handler;
    T * _ptr;
};

__END_UTIL

#endif
