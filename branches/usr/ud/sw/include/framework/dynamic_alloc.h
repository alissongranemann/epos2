// EPOS Component Dynamic Allocation Aspect Program

#ifndef __dynamic_alloc_h
#define __dynamic_alloc_h

#include <system/config.h>
#include <utility/malloc.h>

__BEGIN_SYS

template<typename Obj_Type, bool system_heap>
class Dynamic_Allocation
{
public:
    typedef Obj_Type * Idx_Type;

public:
    Dynamic_Allocation(): _null() {}

    bool allocate(Idx_Type &idx) {
        idx = system_heap ? (new (SYSTEM) Obj_Type()) : (new(malloc(sizeof(Obj_Type))) Obj_Type());
        if(idx)
            return true;
        else {
            idx = &_null;
            return false;
        }
    }

    template<typename T0>
    bool allocate(Idx_Type &idx, T0 &t0) {
        system_heap ? (new (SYSTEM) Obj_Type(t0)) : (new(malloc(sizeof(Obj_Type))) Obj_Type(t0));
        if(idx)
            return true;
        else
            return false;
    }

    template<typename T0, typename T1>
    bool allocate(Idx_Type &idx, T0 &t0, T1 &t1) {
        idx = system_heap ? (new (SYSTEM) Obj_Type(t0, t1)) : (new(malloc(sizeof(Obj_Type))) Obj_Type(t0, t1));
        if(idx)
            return true;
        else {
            idx = &_null;
            return false;
        }
    }

    template<typename T0, typename T1, typename T2>
    bool allocate(Idx_Type &idx, T0 &t0, T1 &t1, T2 &t2) {
        system_heap ? (new (SYSTEM) Obj_Type(t0, t1, t2)) : (new(malloc(sizeof(Obj_Type))) Obj_Type(t0, t1, t2));
        if(idx)
            return true;
        else {
            idx = &_null;
            return false;
        }
    }

    void free(Idx_Type idx) { delete idx; }

    Obj_Type * get(Idx_Type idx){ return idx; }

    Obj_Type * null() { return &_null; }

private:
    Obj_Type _null;
};

__END_SYS

#endif
