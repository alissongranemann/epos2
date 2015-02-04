#ifndef STATIC_UNIFIED_ALLOC_H_
#define STATIC_UNIFIED_ALLOC_H_

namespace Implementation {

template<typename Obj_Type, typename Idx, Idx Max>
class Static_Allocation {

public:
    typedef Idx Idx_Type;


public:
    Static_Allocation()
        : _elements(), _null(&_elements[Max]), _alloc_bitmap() {}

    bool allocate(Idx_Type &idx) {
        bool result = search(idx);
        if(result){
            _elements[idx] = Obj_Type();
        }
        return result;
    }

    template<typename T0>
    bool allocate(Idx_Type &idx, T0 &t0) {
        bool result = search(idx);
        if(result){
            _elements[idx] = Obj_Type(t0);
        }
        return result;
    }

    template<typename T0, typename T1>
    bool allocate(Idx_Type &idx, T0 &t0, T1 &t1) {
        bool result = search(idx);
        if(result){
            _elements[idx] = Obj_Type(t0, t1);
        }
        return result;
    }

    template<typename T0, typename T1, typename T2>
    bool allocate(Idx_Type &idx, T0 &t0, T1 &t1, T2 &t2) {
        bool result = search(idx);
        if(result){
            _elements[idx] = Obj_Type(t0, t1, t2);
        }
        return result;
    }

    void free(Idx_Type idx) {
        _alloc_bitmap[idx] = false;
    }

    Obj_Type* get(Idx_Type idx){
        return &_elements[idx];
    }

    Obj_Type* null(){
        return _null;
    }


private:

    inline bool search(Idx_Type &idx){
        alloc_search: for(Idx_Type i = Idx_Type(); i < Max; ++i)
            if(_alloc_bitmap[i] == false) {
                _alloc_bitmap[i] = true;
                idx = i;
                return true;
            }
        idx = Max;
        return false;
    }

    Obj_Type _elements[Max+1];
    Obj_Type* _null;
    bool _alloc_bitmap[Max+1];

};

}


#endif /* STATIC_ALLOC_H_ */
