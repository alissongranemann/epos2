// EPOS Adder Abstraction Declarations

#ifndef __adder_unified_h
#define __adder_unified_h

__BEGIN_SYS

class Adder
{
private:
    enum {
        EXPANDED_SIZE = 176
    };

public:
    Adder() {}

    ~Adder() {}

    int add(int a, int b) { return (a + b); }

    void rest_st(unsigned int * buf, unsigned int len) {
        for(int i = 0; i < len; i++)
            _key[i] = *(buf++);
    }

    unsigned int save_st() {
        static unsigned int i = 0;

        return _key[i++];
    }

    unsigned int get_st_len() { return EXPANDED_SIZE; }

private:
    unsigned int _key[EXPANDED_SIZE];
};

__END_SYS

#endif
