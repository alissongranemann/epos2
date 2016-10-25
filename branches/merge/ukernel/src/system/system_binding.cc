// EPOS System Binding

#include <machine.h>
#include <display.h>
#include <thread.h>

__USING_SYS;
extern "C" {
    void _panic() { Machine::panic(); }
    void _exit(int s) { Thread::exit(s); }
    void __exit() { Thread::exit(CPU::fr()); }  // must be handled by the Page Fault handler for user-level tasks
    void _print(const char * s) { Display::puts(s); }
    void _cpu_int_enable() { CPU::int_enable(); }
    void _cpu_int_disable() { CPU::int_disable(); }

    // LIBC Heritage
    void __cxa_pure_virtual() {
        db<void>(ERR) << "Pure Virtual method called!" << endl;
    }
}

__BEGIN_UTIL
void OStream::preamble()
{
    static char tag[] = "<0>: ";

    int me = Machine::cpu_id();
    int last = CPU::cas(_lock, -1, me);
    for(int i = 0, owner = last; (i < 10) && (owner != me); i++, owner = CPU::cas(_lock, -1, me));
    if(last != me) {
        tag[1] = '0' + Machine::cpu_id();
        print(tag);
    }
}

void OStream::trailler()
{
    static char tag[] = " :<0>";

    if(_lock != -1) {
        tag[3] = '0' + Machine::cpu_id();
        print(tag);

        _lock = -1;
    }
    if(_error)
        _panic();
}
__END_UTIL
