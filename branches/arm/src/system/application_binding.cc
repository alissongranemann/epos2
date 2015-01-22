// EPOS Application Binding

#include <utility/ostream.h>
#include <application.h>
#include <cpu.h>
#include <framework/main.h>

extern "C" {
    void _panic() {
        _API::Thread::exit(-1); // Could be replaced by a throw, since exceptions should not be a problem for user space
    }

    void _exit(int s) {
        _API::Thread::exit(s);
    }

    void _print(const char * s) {
        _SYS::Message msg(_SYS::Id(_SYS::UTILITY_ID, 0));
        msg.act(_SYS::Message::PRINT, reinterpret_cast<unsigned int>(s));
    }

    int _syscall(void * m) {
        return _SYS::CPU::syscall(m);
    }
}

__BEGIN_SYS

OStream kerr;

__END_SYS
