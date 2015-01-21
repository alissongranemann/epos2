// EPOS System Scaffold and System Abstraction Implementation

#include <utility/ostream.h>
#include <utility/heap.h>
#include <machine.h>
#include <display.h>
#include <system.h>
#include <alarm.h>
#include <thread.h>
#include <framework/main.h>
#include <framework/agent.h>

__USING_SYS;

extern "C" {
    void _panic() { Machine::panic(); }
    void _exit(int s) { Thread::exit(s); }
    void _exec(Agent<void> * agent) { agent->exec(); }
    void _print(const char * s) { Display::puts(s); }

    // LIBC Heritage
    void __cxa_pure_virtual() {
        db<void>(ERR) << "Pure Virtual method called!" << endl;
    }
}
