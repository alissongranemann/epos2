// EPOS Application Scaffold and Application Abstraction Implementation

#include <utility/ostream.h>
#include <application.h>
#include <framework/main.h>

// Application class attributes
__BEGIN_SYS
// System_Info<Machine> * System::_si = reinterpret_cast<System_Info<Machine> *>(Memory_Map<Machine>::SYS_INFO);
char Application::_preheap[];
Heap * Application::_heap;
__END_SYS

// Global objects
__BEGIN_API
OStream cout;
OStream cerr;
__END_API
