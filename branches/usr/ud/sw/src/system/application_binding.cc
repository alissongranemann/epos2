// EPOS Application Binding

#include <utility/ostream.h>
#include <application.h>
#include <cpu.h>
#include <framework/main.h>

// Framework class attributes
__BEGIN_SYS
Framework::Cache Framework::_cache;
__END_SYS


// Global objects
__BEGIN_SYS
OStream kerr;
__END_SYS


// Bindings
extern "C" {
    void _panic() { _API::Thread::exit(-1); }
    void _exit(int s) { _API::Thread::exit(s); }
}

__USING_SYS;
extern "C" {
    int _syscall(void * m) { return CPU::syscall(m); }
    void _print(const char * s) {
        Message msg(Id(UTILITY_ID, 0));
        msg.act(Message::PRINT, reinterpret_cast<unsigned int>(s));
    }
}
