#ifndef diffie_hellman_h
#define diffie_hellman_h

#include <utility/bignum.h>

__BEGIN_SYS;

class Diffie_Hellman
{
	public:
	static const unsigned int SECRET_SIZE = Traits<Diffie_Hellman>::SECRET_SIZE;
	static const unsigned int PUBLIC_KEY_SIZE = Traits<Diffie_Hellman>::PUBLIC_KEY_SIZE;

	static const unsigned char default_base_point_x[Bignum::sz_word];
	static const unsigned char default_base_point_y[Bignum::sz_word];

	struct ECC_Point : private Bignum
	{
		Bignum x, y, z;
		ECC_Point()__attribute__((noinline)){}
		void operator=(const ECC_Point &b)__attribute__((noinline))
		{
			x = b.x;
			y = b.y;
			z = b.z;
		}
		void operator*=(const Bignum &b);
		friend OStream &operator<<(OStream &out, const ECC_Point &a)
		{
			out << "Point: " << endl;
			out << a.x << endl;
			out << a.y << endl;
			out << a.z << endl;
			return out;
		}

		private:
		void jacobian_double();
		void add_jacobian_affine(const ECC_Point &b);
	};

	Diffie_Hellman( const unsigned char base_point_data_x[Bignum::word * Bignum::sz_digit], const unsigned char base_point_data_y[Bignum::word * Bignum::sz_digit])__attribute__((noinline))
		: _base_point()
	{
		_base_point.x.set_bytes(base_point_data_x, Bignum::word * Bignum::sz_digit);
		_base_point.y.set_bytes(base_point_data_y, Bignum::word * Bignum::sz_digit);
		_base_point.z = 1;
		generate_keypair();
	}
	Diffie_Hellman()__attribute__((noinline))
	{
		_base_point.x.set_bytes(default_base_point_x, Bignum::word * Bignum::sz_digit);
		_base_point.y.set_bytes(default_base_point_y, Bignum::word * Bignum::sz_digit);
		_base_point.z = 1;
		generate_keypair();
	}

	private:
	Bignum _private;
	//Bignum _key;
	ECC_Point _base_point;
	ECC_Point _public;

	public:
	int get_public(unsigned char * buffer, unsigned int buffer_size);
	//int get_key(unsigned char * buffer, int buffer_size);
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
	void calculate_key(unsigned char * key, unsigned int buffer_size, const unsigned char * Yb, int Yb_size);
};

__END_SYS;
#endif
