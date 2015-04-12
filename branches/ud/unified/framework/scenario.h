#ifndef __scenario_unified_h
#define __scenario_unified_h

#include <system/config.h>
#include <aspects/static_alloc.h>
#include <aspects/dynamic_alloc.h>

__BEGIN_SYS

template<class T, typename Alloc_Obj_Type, typename Alloc_Idx, unsigned int Alloc_Max>
class HW_Scenario: public Static_Allocation<Alloc_Obj_Type, Alloc_Idx, Alloc_Max>
{
public:
    typedef typename Static_Allocation<Alloc_Obj_Type, Alloc_Idx,
            Alloc_Max>::Idx_Type Link;
};

// No allocation, empty scenario
template<class T>
class HW_Scenario<T, void, void, 0>
{
public:
    typedef void Link;
};

template<class T, typename Alloc_Obj_Type, typename Alloc_Idx, unsigned int Alloc_Max>
class SW_Scenario: public IF<Traits<T>::static_alloc,
                          Static_Allocation<Alloc_Obj_Type, Alloc_Idx, Alloc_Max>,
                          Dynamic_Allocation<Alloc_Obj_Type> >::Result
{
public:
    typedef typename IF<Traits<T>::static_alloc,
                     Static_Allocation<Alloc_Obj_Type, Alloc_Idx, Alloc_Max>,
                     Dynamic_Allocation<Alloc_Obj_Type> >::Result
                     Allocator;
    typedef typename Allocator::Idx_Type Link;
};

// No allocation, empty scenario
template<class T>
class SW_Scenario<T, void, void, 0>
{
public:
    typedef void Link;
};

__END_SYS

#endif
