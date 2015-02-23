// EPOS Aspect Programs

#ifndef __aspect_h
#define __aspect_h

#include <aspect/identified.h>
#include <aspect/shared.h>

__BEGIN_SYS

class Null_Aspect {};

template<typename Aspect, bool active>
class Conditional_Aspect: public Aspect {};

template<typename Aspect>
class Conditional_Aspect<Aspect, false>: public Null_Aspect {};

template<typename Component>
class Aspects2: public Conditional_Aspect<Identified<Component>, Traits<Component>::ASPECTS::template Count<Identified<Component> >::Result != 0> {};

template<typename List>
class Recur_Aspect: public Recur_Aspect<typename List::Tail> {};

template<>
class Recur_Aspect<void> {};

template<typename Component>
class Aspects: public Traits<Component>::ASPECTS::template Recur<Component> { public: Aspects() {} };

__END_SYS

#endif
