#ifndef __static_alloc_unified_h
#define __static_alloc_unified_h

__BEGIN_SYS

template<typename Obj_Type, typename Idx, Idx Max>
class Static_Allocation
{
public:
    typedef Idx Idx_Type;

public:
    Static_Allocation(): _elements(), _null(&_elements[Max]), _alloc_bitmap() {}

    bool allocate(Idx_Type &idx) {
        bool result = search(idx);
        if(result)
            _elements[idx] = Obj_Type();
        return result;
    }

    template<typename T0>
    bool allocate(Idx_Type &idx, T0 &a0) {
        bool result = search(idx);
        if(result)
            _elements[idx] = Obj_Type(a0);
        return result;
    }

    template<typename T0, typename T1>
    bool allocate(Idx_Type &idx, T0 &a0, T1 &a1) {
        bool result = search(idx);
        if(result)
            _elements[idx] = Obj_Type(a0, a1);
        return result;
    }

    template<typename T0, typename T1, typename T2>
    bool allocate(Idx_Type &idx, T0 &a0, T1 &a1, T2 &a2) {
        bool result = search(idx);
        if(result)
            _elements[idx] = Obj_Type(a0, a1, a2);
        return result;
    }

    void free(Idx_Type idx) { _alloc_bitmap[idx] = false; }

    Obj_Type * get(Idx_Type idx) { return &_elements[idx]; }

    Obj_Type * null() { return _null; }

private:
    inline bool search(Idx_Type &idx) {
        alloc_search: for(Idx_Type i = Idx_Type(); i < Max; i++)
            if(_alloc_bitmap[i] == false) {
                _alloc_bitmap[i] = true;
                idx = i;
                return true;
            }
        idx = Max;
        return false;
    }

    Obj_Type _elements[Max + 1];
    Obj_Type * _null;
    bool _alloc_bitmap[Max + 1];
};

__END_SYS

#endif
