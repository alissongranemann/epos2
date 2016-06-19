// EPOS (Litte-endian) Big Numbers Utility Test Program

#include <utility/string.h>
#include <utility/bignum.h>
#include <utility/random.h>
#include <utility/aes.h>
#include <utility/diffie_hellman.h>

using namespace EPOS;

const int ITERATIONS = 5000000;
const int SIZE = 16;
const unsigned int INT_MAX = -1;

OStream cout;

int main()
{
    cout << "Bignum Utility Test" << endl;

    cout << "sizeof(Bignum<" << SIZE << ">) = " << sizeof(Bignum<SIZE>) << " bytes." << endl;

    Bignum<SIZE> a = 0, b = 1;
    cout << "a = " << a << ", b = " << b << endl;
    cout << "a + b = " << a + b << endl;
    cout << "a * b = " << a * b << endl;

    a = 100000000;
    b = 200000000;
    cout << "a = " << a << ", b = " << b << endl;
    cout << "a + b = " << a + b << endl;
    cout << "a * b = " << a * b << endl;

    a = Bignum<SIZE>("10000000000000000000000000000000", 16);
    b = Bignum<SIZE>("20000000000000000000000000000000", 16);
    cout << "a = " << a << ", b = " << b << endl;
    cout << "a + b = " << a + b << endl;

    for(unsigned int i = 0; i < ITERATIONS; i++) {
        /*
		if(!(i%500000))
			cout<<i<<endl;
		r = Random::random();
		a = i;
		b = r;
		a += b;
		k = a.to_uint();
		tst = (i+r == k);
//		cout<<"OKIS1" << endl;
		if(!tst)
		{
			cout << "Error3" << endl;
			cout << i << " " << r << " " << k << endl;
			cout << a << endl;
			cout << b << endl;
			return 1;
		}
         */

        unsigned int r = Random::random();
        a = i;
        b = r;
        a -= b;
        unsigned int k = a;
        if(!((i - r == k) || (r - i == k))) {
            cout << "Error5" << endl;
            cout << i << " " << r << " " << k << " " << i - r << " " << r - i << endl;
            cout << a << endl;
            cout << b << endl;
            return 1;
        }

        /*
		if(i>0)
			r = Random::random()%((INT_MAX)/i);		
		a = i;
		b = r;

		a *= b;
		k = a.to_uint();
		tst = (i*r==k);
//		cout<<"OKIS5" << endl;
		if(!tst)
		{			
			cout << "Error7" << endl;
			cout << i << " " << r << " " << k << " " << i*r << endl;
			cout << a << endl;
			cout << a.to_uint() << endl;
			a = i*r;
			cout << a << endl;
			cout << a.to_uint() << endl;
			cout << b << endl;
			cout << b.to_uint() << endl;
			return 1;
		}
         */

        /*
		r = Random::random()%(INT_MAX-i)+i;
		a = i;
		b = r;
		a /= b;
		k = a.to_uint();
		tst = (i/r==k);
//		cout<<"OKIS7" << endl;
		if(!tst)
		{			
			cout << "Error9" << endl;
			cout << i << " " << r << " " << k << endl;
			cout << a << endl;
			cout << b << endl;
			cout << i/r << endl;
			return 1;
		}

		r = Random::random();
		if(r==0) r++;
		a = i;
		b = r;
		a %= b;
		k = a.to_uint();
		tst = ((i%r)==k);
//		cout<<"OKIS9" << endl;
		if(!tst)
		{			
			cout << "Error11" << endl;
			cout << i << " " << r << " " << k << endl;
			cout << a << endl;
			cout << b << endl;
			cout << i%r << endl;
			return 1;
		}
         */
        //	cout << a << endl;
        //	cout << i << " + " << j << " == " << (int)k << endl;
    }

    cout << "Finish!" << endl;

    return 0;
}
