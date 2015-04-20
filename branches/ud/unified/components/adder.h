// EPOS Adder Abstraction Declarations

#ifndef __adder_unified_h
#define __adder_unified_h

__BEGIN_SYS

class Adder
{
public:
    Adder() {}

    ~Adder() {}

    int add(int a, int b) { return (a + b); }
};

__END_SYS

#endif
