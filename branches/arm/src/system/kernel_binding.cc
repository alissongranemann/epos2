// EPOS Kernel Binding

#include <framework/main.h>
#include <framework/agent.h>

__USING_SYS;
extern "C" { void _exec(Agent<void> * agent) { agent->exec(); }}
