#ifndef diffie_hellman_h
#define diffie_hellman_h

#include <system/config.h>
#include <utility/bignum.h>

__BEGIN_SYS

template <unsigned int SECRET_SIZE = Traits<Diffie_Hellman<0>>::SECRET_SIZE>
class Diffie_Hellman
{
	static const unsigned int PUBLIC_KEY_SIZE = 2 * SECRET_SIZE;

    typedef _UTIL::Bignum<SECRET_SIZE> Bignum;

	static const unsigned char default_base_point_x[SECRET_SIZE]; //TODO: setup
	static const unsigned char default_base_point_y[SECRET_SIZE]; //TODO: setup

	class ECC_Point
	{
    public:
		ECC_Point() __attribute__((noinline)) {}

		void operator*=(const Bignum &b);

		friend OStream &operator<<(OStream &out, const ECC_Point &a) {
			out << "{x= " << a.x << " ,y= " << a.y, " ,z= " << a.z << endl;
			return out;
		}
		friend Debug &operator<<(Debug &out, const ECC_Point &a) {
			out << "{x= " << a.x << " ,y= " << a.y, " ,z= " << a.z << endl;
			return out;
		}

        Diffie_Hellman::Bignum x, y, z;

	private:
		void jacobian_double();
		void add_jacobian_affine(const ECC_Point &b);
	};

public:
	Diffie_Hellman(const unsigned char base_point_data_x[Bignum::word * Bignum::sz_digit], const unsigned char base_point_data_y[Bignum::word * Bignum::sz_digit]) __attribute__((noinline))
	{
		_base_point.x.set_bytes(base_point_data_x, Bignum::word * Bignum::sz_digit);
		_base_point.y.set_bytes(base_point_data_y, Bignum::word * Bignum::sz_digit);
		_base_point.z = 1;
		generate_keypair();
	}

	Diffie_Hellman() __attribute__((noinline))
	{
		_base_point.x.set_bytes(default_base_point_x, Bignum::word * Bignum::sz_digit);
		_base_point.y.set_bytes(default_base_point_y, Bignum::word * Bignum::sz_digit);
		_base_point.z = 1;
		generate_keypair();
	}

	int get_public(unsigned char * buffer)
    {
        unsigned int i, j;
        for(i=0, j=0; i<PUBLIC_KEY_SIZE; i++, j++)
            buffer[j] = _public.x.byte_data[i];
        for(i=0; i<PUBLIC_KEY_SIZE; i++, j++)
            buffer[j] = _public.y.byte_data[i];
        return j;
    }

	void generate_keypair()
	{
		db<Diffie_Hellman>(TRC) << "Diffie_Hellman::generate_keypair()" << endl;
		_private.random();
		db<Diffie_Hellman>(INF) << "Diffie_Hellman Private: " << _private << endl;
		_public = _base_point;
		db<Diffie_Hellman>(INF) << "Diffie_Hellman Base Point: " << _base_point << endl;
		_public *= _private;
		db<Diffie_Hellman>(INF) << "Diffie_Hellman Public: " << _public << endl;
	}

	void calculate_key(unsigned char * key, const unsigned char * Yb);

private:
	Bignum _private;
	ECC_Point _base_point;
	ECC_Point _public;
};

__END_SYS

#endif
