#ifndef __bignum_h
#define __bignum_h

#include <utility/random.h>
#include <cpu.h>

// #define DEBUG_BIGNUM

__BEGIN_UTIL;

class Bignum
{
    public:
    typedef Traits<Bignum>::digit digit;
    typedef Traits<Bignum>::double_digit double_digit;
    static const unsigned int sz_digit = Traits<Bignum>::sz_digit;
    static const unsigned int word = Traits<Bignum>::word;
    static const unsigned int sz_word = Traits<Bignum>::sz_word;
    static const unsigned int double_word = Traits<Bignum>::double_word;
    static const unsigned int bits_in_digit = Traits<Bignum>::bits_in_digit;

//     static const digit default_mod[word];
//     static const digit default_barrett_u[word+1];
    static const unsigned char default_mod[sz_word];
    static const unsigned char default_barrett_u[sz_word + sz_digit];

    union
    {
        digit data[word];
        unsigned char byte_data[sz_word];
    };

    protected:

    // All operations will be done modulo _mod
    const digit * _mod;
    // Auxiliary constant to help multiplication
    const digit * _barrett_u;

    public:
    unsigned int to_uint()
    {
        return data[0];
    }

    // -------------------------------------------------
    // Sets data to  a random number, smaller than _mod
    // -------------------------------------------------
    void random() __attribute__((noinline))
    {
        int i;
        for(i=word-1; i>=0 && (_mod[i] == 0); i--) 
            data[i]=0;
        data[i] = Random::random() % _mod[i];
        for(--i;i>=0;i--)
            data[i] = Random::random();
    }

    // -----------------------------------------------------------
    // Sets data using a little-endian byte-by-byte representation
    // -----------------------------------------------------------
    inline unsigned int set_bytes(const unsigned char * values, unsigned int len)
    {
        unsigned int i, j=0;
        for(i=0; i<word; i++)
        {
            data[i] = 0;
            for(unsigned int k=0; k<sz_digit && j<len; k++, j++)
            {
                data[i] += (values[j] << (8*k));
//                 data[i] *= 256;
            }
        }
    /*
        unsigned int i;
        for(i=0; i<len && i<sz_word; i++)
            byte_data[i] = values[i];
        for(;i<sz_word;i++)
            byte_data[i] = 0;
            */
        /*
        if(Traits<CPU>::ENDIANESS == Traits<CPU>::LITTLE)
        {
            unsigned int j,k;
            for(j=0, k=sz_word-1;j<k;j++,k--)
            {
                byte_data[j] ^= byte_data[k];
                byte_data[k] ^= byte_data[j];
                byte_data[j] ^= byte_data[k];
            }
        }
        */
        return i * sz_digit;
    }

    // -------------------------------------------------------------------
    // -Sets the module to be applied in every operation on this object
    // -The user must handle allocation
    // -To be used only when a module different than the default is needed
    // -------------------------------------------------------------------
    inline void set_mod(digit * mod, digit * barrett_u) 
    { 
        _mod = mod; 
        _barrett_u = barrett_u; 
    }   

    friend OStream &operator<< (OStream &out, const Bignum &b) 
    {
        unsigned int i;
        out << '[';
        for(i=0;i<word;i++)
        {
            out << (unsigned int)b.data[i];
            if(i < word-1)
                out << ", ";
        }
        out << "]";
        return out; 
    }

    public:
    // -----------------------------------------------------------
    // Sets data using a little-endian byte-by-byte representation
    // -----------------------------------------------------------
    Bignum(unsigned char * bytes, unsigned int len)
    {
        set_bytes(bytes, len);
        _mod = (unsigned int *)default_mod; 
        _barrett_u = (unsigned int *)default_barrett_u; 
    }

    // -------------------------
    // Convert from unsigned int
    // -------------------------
    Bignum(unsigned int n = 0) __attribute__((noinline))
    { 
        *this = n;
        _mod = (unsigned int *)default_mod; 
        _barrett_u = (unsigned int *)default_barrett_u; 
    }

    //-------------------------------------
    // Returns true if this number is even,
    // false otherwise
    //-------------------------------------
    inline bool is_even(){ return !(data[0] % 2); }

    // ------------
    // XOR operator
    // ------------
    inline void operator^=(const Bignum &b) //__attribute__( ( noinline ) )
    {
        for(unsigned int i=0; i<word; i++)
            data[i] ^= b.data[i];
    }

    // -------------------------
    // Convert from unsigned int
    // -------------------------
    inline void operator=(unsigned int n) //__attribute__( ( noinline ) )
    {
        /*
        // Doing bit-by-bit to avoid compiler warning below, which is treated as an error
        // TODO: THIS IS WRONG!
        for(unsigned int i=0;i<word;i++)
        {
            data[i] = 0;
            for(unsigned int j=0;j<bits_in_digit;j++)
            {
                data[i] += (n % 2);
                n >>=1;
            }
        }
        */
        if(sizeof(n) <= sz_digit)
        {
            data[0] = n;
            for(unsigned int i=1;i<word;i++)
                data[i] = 0;
        }
        /*
        else
        {
            unsigned int i;
            for(i=0; (n != 0) && (i<word); i++)
            {
                data[i] = n;
                n >>= bits_in_digit; //Compiler warning can be ignored because of the 'if'
            }
            for(;i<word;i++)
                data[i] = 0;
        }
        */
    }

    // ----------------
    // Copies data only
    // ----------------
    inline void operator=(const Bignum &b) //__attribute__( ( noinline ) )
    {
        for(unsigned int i=0;i<word;i++)
            data[i] = b.data[i];
    }

    // --------------------
    // Comparison operators
    // --------------------
    inline bool operator==(const Bignum &b) const //__attribute__( ( noinline ) )
    { return (cmp(data, b.data, word) == 0); }
    inline bool operator!=(const Bignum &b) const //__attribute__( ( noinline ) )
    { return (cmp(data, b.data, word) != 0); }
    inline bool operator>=(const Bignum &b) const //__attribute__( ( noinline ) )
    { return (cmp(data, b.data, word) >= 0); }
    inline bool operator<=(const Bignum &b) const //__attribute__( ( noinline ) )
    { return (cmp(data, b.data, word) <= 0); }
    inline bool operator>(const Bignum &b) const //__attribute__( ( noinline ) )
    { return (cmp(data, b.data, word) > 0); }
    inline bool operator<(const Bignum &b) const //__attribute__( ( noinline ) )
    { return (cmp(data, b.data, word) < 0); }

    // --------------------------------
    // data = (data * b.data) % _mod
    // --------------------------------
    void operator*=(const Bignum &b)__attribute__((noinline))
    {
        if(b == 1) return;
#ifdef DEBUG_BIGNUM
        kout << "BIGNUM_DEBUG_EQUATION_START" << endl;
        kout << *this << endl;
        kout << "*" << endl;
        kout << b << endl;
        kout << "%" << endl;
        kout << '[';
        for(unsigned int i=0;i<word-1;i++)
            kout << _mod[i] << ", ";
        kout << _mod[word-1] << ']' << endl;
        kout << "==" << endl;
#endif
        digit _mult_result[double_word];

        // ------------------------------
        // _mult_result = data * b.data
        // ------------------------------
        simple_mult(_mult_result, data, b.data, word);

        // -------------------------
        // Barrett modular reduction
        // -------------------------
        barrett_reduction(data, _mult_result, word);
#ifdef DEBUG_BIGNUM
        kout << *this << endl;
#endif
    }

    // --------------------------------
    // data = (data + b.data) % _mod
    // --------------------------------
    void operator+=(const Bignum &b)__attribute__((noinline))
    {
#ifdef DEBUG_BIGNUM
        kout << "BIGNUM_DEBUG_EQUATION_START" << endl;
        kout << *this << endl;
        kout << "+" << endl;
        kout << b << endl;
        kout << "%" << endl;
        kout << '[';
        for(unsigned int i=0;i<word-1;i++)
            kout << _mod[i] << ", ";
        kout << _mod[word-1] << ']' << endl;
        kout << "==" << endl;
#endif
        if(simple_add(data, data, b.data, word))
            simple_sub(data, data, _mod, word);
        if(cmp(data, _mod, word) >= 0)
            simple_sub(data, data, _mod, word);
#ifdef DEBUG_BIGNUM
        kout << *this << endl;
#endif
    }

    // --------------------------------
    // data = (data - b.data) % _mod
    // --------------------------------
    void operator-=(const Bignum &b)__attribute__((noinline))
    {
#ifdef DEBUG_BIGNUM
        kout << "BIGNUM_DEBUG_EQUATION_START" << endl;
        kout << *this << endl;
        kout << "-" << endl;
        kout << b << endl;
        kout << "%" << endl;
        kout << '[';
        for(unsigned int i=0;i<word-1;i++)
            kout << _mod[i] << ", ";
        kout << _mod[word-1] << ']' << endl;
        kout << "==" << endl;
#endif
        if(simple_sub(data, data, b.data, word))
            simple_add(data, data, _mod, word);
#ifdef DEBUG_BIGNUM
        kout << *this << endl;
#endif
    }

    //-------------------------------------------------------------
    // Calculates the modular multiplicative inverse of this number
    //-------------------------------------------------------------
    void inverse() __attribute__((noinline))
    {
        Bignum A(1), u, v, zero(0);
        for(unsigned int i=0;i<word;i++)
        {
            u.data[i] = data[i];
            v.data[i] = _mod[i];
        }
        *this = 0;
        while(u != zero)
        {
            while(u.is_even())
            {
                u.divide_by_two();
                if(A.is_even())
                    A.divide_by_two();
                else
                {
                    bool carry = simple_add(A.data, A.data, _mod, word);
                    A.divide_by_two(carry);             
                }
            }
            while(v.is_even())
            {
                v.divide_by_two();
                if(is_even())
                    divide_by_two();
                else
                {
                    bool carry = simple_add(data, data, _mod, word);
                    divide_by_two(carry);
                }
            }
            if(u >= v)
            {
                u -= v;
                A -= *this;
            }
            else
            {
                v-=u;
                *this -= A;
            }
        }
    }

    //----------------------------------------------------------------
    // Shift left (actually shift right, because of little endianness)
    // - Does not apply modulo
    // - Returns carry bit
    //----------------------------------------------------------------
    bool multiply_by_two(bool carry = 0)__attribute__((noinline))
    {
#ifdef DEBUG_BIGNUM
        bool debug = !carry;
        if(debug)
        {
            kout << "BIGNUM_DEBUG_EQUATION_START" << endl;      
            kout << *this << endl;
            kout << "<<" << endl;
            kout << "[1]" << endl;
            kout << "%" << endl;
            kout << '[';
            for(unsigned int i=0;i<word-1;i++)
                kout << _mod[i] << ", ";
            kout << _mod[word-1] << ']' << endl;
            kout << "==" << endl;
        }
#endif
        bool next_carry;
        for(unsigned int i=0;i<word;i++)
        {
            next_carry = data[i] >> (bits_in_digit-1);
            data[i] <<= 1;
            data[i] += (digit)carry;
            carry = next_carry;
        }
#ifdef DEBUG_BIGNUM
        if(debug)
            kout << *this << endl;
#endif
        return carry;
    }

    //----------------------------------------------------------------
    // Shift right (actually shift left, because of little endianness)
    // - Does not apply modulo
    // - Returns carry bit
    //----------------------------------------------------------------
    bool divide_by_two(bool carry = 0)__attribute__((noinline))
    {
#ifdef DEBUG_BIGNUM
        bool debug = !carry;
        if(debug)
        {
            kout << "BIGNUM_DEBUG_EQUATION_START" << endl;      
            kout << *this << endl;
            kout << ">>" << endl;
            kout << "[1]" << endl;
            kout << "%" << endl;
            kout << '[';
            for(unsigned int i=0;i<word-1;i++)
                kout << _mod[i] << ", ";
            kout << _mod[word-1] << ']' << endl;
            kout << "==" << endl;
        }
#endif
        bool next_carry;
        for(int i=word-1;i>=0;i--)
        {
            next_carry = data[i] % 2;
            data[i] >>= 1;
            data[i] += (digit)carry << (bits_in_digit-1);
            carry = next_carry;
        }
#ifdef DEBUG_BIGNUM
        if(debug)
            kout << *this << endl;
#endif
        return carry;
    }

    protected:
    //--------------------
    // C-like comparison
    // Returns: 
    // 1  if a >  b
    // -1 if a <  b
    // 0  if a == b
    //--------------------
    static inline int cmp(const digit * a, const digit *b, int size)
    {
        for(int i=size-1; i>=0; i--)
        {
            if(a[i] > b[i]) return 1;
            else if(a[i] < b[i]) return -1;
        }
        return 0;
    }

    // -------------------------------------------------
    // res = a - b 
    // returns: borrow bit
    // -No modulo applied
    // -a, b and res are assumed to have size 'size'
    // -a, b, res are allowed to point to the same place
    // -------------------------------------------------
    static inline bool simple_sub(digit * res, const digit * a, const digit * b, unsigned int size)__attribute__((noinline))
    {
        double_digit borrow = 0;
        double_digit aux = ((double_digit)1) << bits_in_digit;
        for(unsigned int i=0; i<size; i++)
        {
            double_digit anow = a[i];
            double_digit bnow = ((double_digit)b[i]) + borrow;
            borrow = (anow < bnow);
            if(borrow)
                res[i] = (aux - bnow) + anow;   
            else
                res[i] = anow - bnow;   
        }
        return borrow;
    }

    // -------------------------------------------------
    // res = a + b 
    // returns: carry bit
    // -No modulo applied
    // -a, b and res are assumed to have size 'size'
    // -a, b, res are allowed to point to the same place
    // -------------------------------------------------
    static inline bool simple_add(digit * res, const digit * a, const digit * b, unsigned int size)__attribute__((noinline))
    {
        bool carry = 0;
        for(unsigned int i=0; i<size; i++)
        {
            double_digit tmp = (double_digit)carry + (double_digit)a[i] + (double_digit)b[i];
            res[i] = tmp;
            carry = tmp >> bits_in_digit;
        }
        return carry;
    }

    // ------------------------------------------
    // res = (a * b)
    // - Does not apply module
    // - a and b are assumed to be of size 'size'
    // - res is assumed to be of size '2*size'
    // ------------------------------------------
    static inline void simple_mult(digit * res, const digit * a, const digit * b, unsigned int size)
    {
        unsigned int i;
        double_digit r0=0, r1=0, r2=0;
        for(i=0;i<size*2-1;i++)
        {
            for(unsigned int j=0;(j<size) && (j<=i);j++)
            {
                unsigned int k = i - j;
                if(k<size)
                {
                    double_digit aj = a[j];
                    double_digit bk = b[k];
                    double_digit prod = aj * bk;
                    r0 += (digit)prod;
                    r1 += (prod >> bits_in_digit) + (r0 >> bits_in_digit);
                    r0 = (digit)r0;
                    r2 += (r1 >> bits_in_digit);
                    r1 = (digit)r1;
                }
            }
            res[i] = r0;
            r0 = r1;
            r1 = r2;
            r2 = 0;
        }
        res[i] = r0;
    }

    // --------------------------------------------
    // res = a % _mod
    // - Intended to be used after a multiplication
    // - res is assumed to be of size 'size'
    // - a is assumed to be of size '2*size'
    // --------------------------------------------
    inline void barrett_reduction(digit * res, const digit * a, unsigned int size)
    {
        digit q[size+1];

        // --------------------------------------------------------------------
        // q = floor( ( floor( a/base^(size-1) ) * barrett_u ) / base^(size+1))
        // --------------------------------------------------------------------
        double_digit r0=0, r1=0, r2=0;
        unsigned int i;
        for(i=0; i<(2*(size+1))-1 ;i++)
        {
            for(unsigned int j=0;(j<size+1) && (j<=i);j++)
            {
                unsigned int k = i - j;
                if(k<size+1)
                {
                    // shifting left (little endian) size-1 places
                    // a is assumed to have size size*2
                    double_digit aj = a[j+(size-1)];
                    double_digit bk = _barrett_u[k];
                    double_digit prod = aj * bk;
                    r0 += (digit)prod;
                    r1 += (prod >> bits_in_digit) + (r0 >> bits_in_digit);
                    r0 = (digit)r0;
                    r2 += r1 >> bits_in_digit;
                    r1 = (digit)r1;
                }
            }
            // shifting left (little endian) size+1 places
            if(i>=size+1)
                q[i-(size+1)] = r0;
            r0 = r1;
            r1 = r2;
            r2 = 0;
        }
        q[i-(size+1)] = r0;

        digit r[size+1];
        // ------------------------------
        // r = (q * _mod) % base^(size+1)
        // ------------------------------
        r0=0, r1=0, r2=0;
        for(i=0;i<size+1;i++)
        {
            for(unsigned int j=0;j<=i;j++)
            {
                unsigned int k = i - j;
                double_digit aj = q[j];
                double_digit bk = (k == size ? 0 : _mod[k]);
                double_digit prod = aj * bk;
                r0 += (digit)prod;
                r1 += (prod >> bits_in_digit) + (r0 >> bits_in_digit);
                r0 = (digit)r0;
                r2 += r1 >> bits_in_digit;
                r1 = (digit)r1;
            }
            r[i] = r0;
            r0 = r1;
            r1 = r2;
            r2 = 0;
        }

        // --------------------------------------------------------
        // r = ((a % base^(size+1)) - r) % base^(size+1)
        // --------------------------------------------------------
        simple_sub(r, a, r, size+1);

        // ----------------
        // data = r % _mod
        // ----------------
        while((r[size] > 0) || (cmp(r, _mod, size) >= 0))
        {
            if(simple_sub(r, r, _mod, size))
                r[size]--;
        }

        for(unsigned int i=0;i<size;i++)
            res[i] = r[i];
    }
};
__END_UTIL;
#endif
