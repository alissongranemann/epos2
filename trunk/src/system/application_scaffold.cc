// EPOS Application Scaffold and Application Abstraction Implementation

#include <application.h>
#include <framework/main.h>

// Application class attributes
__BEGIN_SYS
char Application::_preheap[];
Heap * Application::_heap;
Framework::Cache Framework::_cache;
__END_SYS

// Global objects
__BEGIN_API
OStream cout;
OStream cerr;
__END_API
