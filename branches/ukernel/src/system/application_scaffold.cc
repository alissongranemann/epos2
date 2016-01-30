// EPOS Application Scaffold and Application Abstraction Implementation

#include <utility/ostream.h>
#include <application.h>
#include <big_kernel_lock.h>

__BEGIN_SYS

// Application class attributes
char Application::_preheap[];
Heap * Application::_heap;

// Big_Kernel_Lock class attributes
volatile unsigned int Big_Kernel_Lock::_level = 0;
volatile int Big_Kernel_Lock::_owner = 0;
int * Big_Kernel_Lock::_running_thread = reinterpret_cast<int *>(Memory_Map<PC>::SYS_SHARED);

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
