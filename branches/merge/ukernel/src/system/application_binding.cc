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

__BEGIN_UTIL
void OStream::preamble() {}
void OStream::trailler() {}

unsigned int This_Thread::id() { return reinterpret_cast<volatile unsigned int>(_API::Thread::self()); }
__END_UTIL

__USING_SYS;
extern "C" {
    void _syscall(void * m) { CPU::syscall(m); }
    void _print(const char * s) {
        Message msg(Id(UTILITY_ID, 0), Message::PRINT, reinterpret_cast<unsigned int>(s));
        msg.act();
    }
}
