#include <diffie_hellman.h>

__BEGIN_SYS

// Class attributes
template<>
const unsigned char Diffie_Hellman<16>::default_base_point_x[16] = 
{       
    0x86, 0x5B, 0x2C, 0xA5,
    0x7C, 0x60, 0x28, 0x0C,
    0x2D, 0x9B, 0x89, 0x8B,
    0x52, 0xF7, 0x1F, 0x16
};
template<>
const unsigned char Diffie_Hellman<16>::default_base_point_y[16] =
{
    0x83, 0x7A, 0xED, 0xDD,
    0x92, 0xA2, 0x2D, 0xC0,
    0x13, 0xEB, 0xAF, 0x5B,
    0x39, 0xC8, 0x5A, 0xCF
};

// Methods
template <unsigned int SECRET_SIZE>
void Diffie_Hellman<SECRET_SIZE>::calculate_key(unsigned char * key, const unsigned char * Yb)
{
    ECC_Point _Y;
    unsigned int i;
    i = _Y.x.set_bytes(Yb, SECRET_SIZE);
    _Y.y.set_bytes(Yb+i, SECRET_SIZE-i);
    _Y.z = 1;
    db<Diffie_Hellman>(TRC) << "Diffie_Hellman - Calculating key: Yb: " << _Y << endl;
    db<Diffie_Hellman>(TRC) << "private: " << _private << endl;
    
    _Y *= _private;
    db<Diffie_Hellman>(TRC) << "Diffie_Hellman - Key set: Y: " << _Y << endl;

    _Y.x ^= _Y.y;

    for(i=0; i<SECRET_SIZE; i++)
        key[i] = _Y.x.byte_data[i];

    db<Diffie_Hellman>(INF) << "Diffie_Hellman - Key set: " << _Y.x << endl;
}

template <unsigned int SECRET_SIZE>
void Diffie_Hellman<SECRET_SIZE>::ECC_Point::operator*=(const Bignum &b)
{
    // Finding last '1' bit of k
    unsigned int t = Bignum::bits_in_digit;
    int b_len = Bignum::word+1;
    typename Bignum::Digit now; //= x._data[Bignum::word - 1];   
    do {
        now = b.data[(--b_len)-1];
    }while(now == 0);
    assert(b_len > 0);

    bool bin[t]; // Binary representation of now

    ECC_Point pp(*this);
    
    for(int j=Bignum::bits_in_digit-1;j>=0;j--) {
        if(now%2) 
            t=j+1;
        bin[j] = now%2;
        now/=2;
    }

    for(int i=b_len-1;i>=0;i--) {
        for(;t<Bignum::bits_in_digit;t++) {
            jacobian_double();
            if(bin[t]) {
                add_jacobian_affine(pp);
            }
        }
        if(i>0) {
            now = b.data[i-1];
            for(int j=Bignum::bits_in_digit-1;j>=0;j--) {
                bin[j] = now%2;
                now/=2;
            }
            t=0;
        }
    }

    Bignum Z; 
    z.inverse();
    Z = z; 
    Z *= z;

    x *= Z;
    Z *= z;

    y *= Z;
    z = 1;
}

template <unsigned int SECRET_SIZE>
void Diffie_Hellman<SECRET_SIZE>::ECC_Point::add_jacobian_affine(const ECC_Point &b)
{
    Bignum A(z), B, C, X, Y, aux, aux2;  

    A *= z;

    B = A;

    A *= b.x;

    B *= z;
    B *= b.y;

    C = A;
    C -= x;

    B -= y;
    
    X = B;
    X *= B;
    aux = C;
    aux *= C;

    Y = aux;

    aux2 = aux;
    aux *= C;
    aux2 *= 2;
    aux2 *= x;
    aux += aux2;
    X -= aux;

    aux = Y;
    Y *= x;
    Y -= X;
    Y *= B;
    aux *= y;
    aux *= C;
    Y -= aux;

    z *= C;

    x = X;
    y = Y;  
}

template <unsigned int SECRET_SIZE>
void Diffie_Hellman<SECRET_SIZE>::ECC_Point::jacobian_double()
{
    Bignum B, C(x), aux(z);

    aux *= z;
    C -= aux;
    aux += x;
    C *= aux;
    C *= 3;

    z *= y;
    z *= 2;

    y *= y;
    B = y;

    y *= x;
    y *= 4;

    B *= B; 
    B *= 8;
        
    x = C;
    x *= x;
    aux = y;
    aux *= 2;
    x -= aux;
    
    y -= x;
    y *= C;
    y -= B; 
}

__END_SYS
