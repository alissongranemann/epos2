// EPOS Metaprograms

#ifndef __meta_unified_h
#define __meta_unified_h

// FIXME: Catapult doesn't support variadic templates so we can't use
// sw/include/system/meta.h

__BEGIN_SYS

// Conditional Type
template<bool condition, typename Then, typename Else>
struct IF
{ typedef Then Result; };

template<typename Then, typename Else>
struct IF<false, Then, Else>
{ typedef Else Result; };


// Conditional Integer
template<bool condition, int Then, int Else>
struct IF_INT
{ enum { Result = Then }; };

template<int Then, int Else>
struct IF_INT<false, Then, Else>
{ enum { Result = Else }; };


// SWITCH-CASE of Types
const int DEFAULT = ~(~0u >> 1); // Initialize with the smallest int

struct Nil_Case {};

template<int tag_, typename Type_, typename Next_ = Nil_Case>
struct CASE
{
    enum { tag = tag_ };
    typedef Type_ Type;
    typedef Next_ Next;
};

template<int tag, typename Case>
class SWITCH
{
    typedef typename Case::Next Next_Case;
    enum {
        case_tag = Case::tag,
        found = ( case_tag == tag || case_tag == DEFAULT  )
    };
public:
    typedef typename IF<found, typename Case::Type,
        typename SWITCH<tag, Next_Case>::Result>::Result Result;
};

template<int tag>
class SWITCH<tag, Nil_Case>
{
public:
    typedef Nil_Case Result;
};


// EQUALty of Types
template<typename T1, typename T2>
struct EQUAL
{ enum { Result = false }; };

template<typename T>
struct EQUAL<T, T>
{ enum { Result = true }; };

template<int T1, int T2>
struct DIV_ROUNDUP
{
    enum {
        _aux = T1/T2,
        _aux_mod = T1%T2,
        Result = IF_INT<(_aux_mod != 0),(_aux + 1),_aux>::Result
    };
};


// LIST of Types
template <typename T1 = void, typename T2 = void, typename T3 = void,
    typename T4 = void, typename T5 = void, typename T6 = void>
class LIST
{
protected:
    typedef T1 Head;
    typedef LIST<T2, T3, T4, T5, T6> Tail;

public:
    enum { Length = Tail::Length + 1 };

    template<int Index, int Current = 0, bool Stop = (Index == Current)>
    struct Get
    { typedef typename Tail::template Get<Index, Current + 1>::Result Result; };

    template<int Index, int Current>
    struct Get<Index, Current, true>
    { typedef Head Result; };

    template<typename Type>
    struct Count
    { enum { Result = EQUAL<Head, Type>::Result + Tail::template Count<Type>::Result }; };

    enum { Polymorphic = (int(Length) != int(Count<Head>::Result)) };
};

template<>
struct LIST<void, void, void, void, void, void>
{
    enum { Length = 0 };

    template<int Index, int Current = 0>
    struct Get
    { typedef void Result; };

    template<typename Type>
    struct Count
    { enum { Result = 0 }; };

    enum { Polymorphic = false };
};

__END_SYS

#endif