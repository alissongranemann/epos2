// EPOS Application Scaffold and Application Abstraction Implementation

#include <utility/ostream.h>
#include <application.h>
#include <big_kernel_lock.h>

__BEGIN_SYS

// Application class attributes
char Application::_preheap[];
Segment * Application::_heap_segment;
Heap * Application::_heap;

// Big_Kernel_Lock class attributes

__END_SYS

__BEGIN_API

// Global objects
__USING_UTIL
OStream cout;
OStream cerr;

__END_API

__BEGIN_UTIL
EPOS::S::U::OStream::Endl endl;
__END_UTIL


#include <aux_debug.h>

void inspect_elf(void * elf) {}
void inspect_as(void * as) {}
void inspect_cs(void * cs) {}
void inspect_code(void * code) {}
void inspect_ds(void * ds) {}
void inspect_data(void * data) {}
void inspect_entry(int (* entry)()) {}
void inspect_task(void * task) {}
void inspect_comm(void * comm) {}
