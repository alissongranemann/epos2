// EPOS Memory Allocation Utility Test Program

#include "../../unified/aspects/dynamic_alloc.h"
#include "../../unified/aspects/static_alloc.h"
#include <utility/ostream.h>

using namespace Implementation;
__USING_SYS;

class Data_Type {
public:
    unsigned int a;
    unsigned int b;

    Data_Type() :a(0), b(0){}

    Data_Type(unsigned int _a, unsigned int _b) :a(_a), b(_b){}
};

typedef Implementation::Dynamic_Allocation<Data_Type> Dynamic_t;
typedef Implementation::Static_Allocation<Data_Type, int, 4> Static_t;

Dynamic_t dyn;
Static_t stat;

template<class Allocator>
void allocate(Allocator &alloc, typename Allocator::Idx_Type &idx){
    OStream cout;
    bool result = alloc.allocate(idx);
    if(result)
        cout << "Allocation successful ID=" << idx << "\n";
    else
        cout << "Allocation failed\n";
}

template<class Allocator>
void deallocate(Allocator &alloc, typename Allocator::Idx_Type &idx){
    OStream cout;
    cout << "Deallocating ID=" << idx << "\n";
    alloc.free(idx);
}

template<class Allocator, typename T0, typename T1>
void allocate(Allocator &alloc, typename Allocator::Idx_Type &idx, T0 t0, T1 t1){
    OStream cout;
    bool result = alloc.allocate(idx, t0, t1);
    if(result)
        cout << "Allocation successful ID=" << idx << "\n";
    else
        cout << "Allocation failed\n";
}


void dynamic_alloc(){
    OStream cout;

    cout << "Dynamically allocating stuff\n";

    Dynamic_t::Idx_Type idx[5];

    allocate(dyn, idx[0]);
    allocate(dyn, idx[1]);
    allocate(dyn,idx[2],1,2);
    allocate(dyn,idx[3],3,4);
    allocate(dyn,idx[4],5,6);

    for (int i = 0; i < 5; ++i) {
        cout << "Obj " << i << " a=" << dyn.get(idx[i])->a << " b=" << dyn.get(idx[i])->b << "\n";
    }

    deallocate(dyn,idx[0]);
    deallocate(dyn,idx[3]);

    allocate(dyn,idx[0],11,11);
    allocate(dyn,idx[3],22,22);

    for (int i = 0; i < 5; ++i) {
        cout << "Obj " << i << " a=" << dyn.get(idx[i])->a << " b=" << dyn.get(idx[i])->b << "\n";
    }


}

void static_alloc(){
    OStream cout;

    cout << "Statically allocating stuff\n";

    Static_t::Idx_Type idx[5];

    allocate(stat, idx[0]);
    allocate(stat, idx[1]);
    allocate(stat,idx[2],1,2);
    allocate(stat,idx[3],3,4);
    allocate(stat,idx[4],5,6);

    for (int i = 0; i < 5; ++i) {
        cout << "Obj " << i << " a=" << stat.get(idx[i])->a << " b=" << stat.get(idx[i])->b << "\n";
    }

    deallocate(stat,idx[0]);
    deallocate(stat,idx[3]);

    allocate(stat,idx[0],11,11);
    allocate(stat,idx[3],22,22);

    for (int i = 0; i < 5; ++i) {
        cout << "Obj " << i << " a=" << stat.get(idx[i])->a << " b=" << stat.get(idx[i])->b << "\n";
    }


}


int main()
{

    dynamic_alloc();
    static_alloc();

    *((volatile unsigned int*)0xFFFFFFFC) = 0;

    return 0;
}
