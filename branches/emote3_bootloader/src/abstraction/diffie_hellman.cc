#include <diffie_hellman.h>
//#include <utility/malloc.h>

__USING_SYS;

int Diffie_Hellman::get_public(unsigned char * buffer, unsigned int buffer_size)
{
    unsigned int i, j;
    for(i=0, j=0; i<Bignum::sz_word && j<buffer_size; i++, j++)
        buffer[j] = _public.x.byte_data[i];
    for(i=0; i<Bignum::sz_word && j<buffer_size; i++, j++)
        buffer[j] = _public.y.byte_data[i];
    return j;
}

/*
int Diffie_Hellman::get_key(unsigned char * buffer, int buffer_size)
{
    int i;
    for(i=0; i<Bignum::sz_word && i<buffer_size; i++)
        buffer[i] = _key.byte_data[i];
    return i;
}
*/

void Diffie_Hellman::calculate_key(unsigned char * key, unsigned int buffer_size, const unsigned char * Yb, int Yb_size)
{
    ECC_Point _Y;
    unsigned int i;
    i = _Y.x.set_bytes(Yb, Yb_size);
    _Y.y.set_bytes(Yb+i, Yb_size-i);
    _Y.z = 1;
    db<Diffie_Hellman>(TRC) << "Diffie_Hellman - Calculating key: Yb: " << _Y << endl;
    db<Diffie_Hellman>(TRC) << "private: " << _private << endl;
    
    _Y *= _private;
    db<Diffie_Hellman>(TRC) << "Diffie_Hellman - Key set: Y: " << _Y << endl;

    _Y.x ^= _Y.y;

    for(i=0; i<Bignum::sz_word && i<buffer_size; i++)
        key[i] = _Y.x.byte_data[i];

    db<Diffie_Hellman>(INF) << "Diffie_Hellman - Key set: " << _Y.x << endl;
}

void Diffie_Hellman::ECC_Point::operator*=(const Bignum &b)
{
    // Finding last '1' bit of k
    unsigned int t = bits_in_digit;
    int b_len = word+1;
    Bignum::digit now; //= x._data[word - 1];   
    do
    {
        now = b.data[(--b_len)-1];
    }while(now == 0);
    if(b_len <= 0)
    {
        kout << "diffie_hellman.cc ERROR 50" << endl;
        while(1);
    }

    bool bin[t]; // Binary representation of now
    //Reducing stack usage
    ECC_Point pp;
    
    pp = *this;
    
    for(int j=bits_in_digit-1;j>=0;j--)
    {
        if(now%2) t=j+1;
        bin[j] = now%2;
        now/=2;
    }

    for(int i=b_len-1;i>=0;i--)
    {
        for(;t<bits_in_digit;t++)
        {
            jacobian_double();
            if(bin[t])
            {
                ;
                add_jacobian_affine(pp);
            }
        }
        if(i>0)
        {
            now = b.data[i-1];
            for(int j=bits_in_digit-1;j>=0;j--)
            {
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

void Diffie_Hellman::ECC_Point::add_jacobian_affine(const ECC_Point &b)
{
    Bignum A;  
    Bignum B;  
    Bignum C;  
    Bignum X;  
    Bignum Y;  
    Bignum aux;
    Bignum aux2;

    A = z; 
    A *= z;

    B = A;

    A *= b.x;

    B *= z;
    B *= b.y;
    //kout << B << endl;
    //kout << b.y << endl; 
    //B.debug_mult(b.y);
    //kout << B << endl << endl << endl;    

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

void Diffie_Hellman::ECC_Point::jacobian_double()
{
    Bignum B, C, aux;
        
    C = x;
    aux = z;
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
