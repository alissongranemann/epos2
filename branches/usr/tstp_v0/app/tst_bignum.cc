#include <utility/string.h>
#include <alarm.h>
#include <utility/bignum.h>
#include <utility/random.h>

__USING_SYS;

OStream cout;

int main()
{
	Alarm::delay(2000000);
	unsigned int i,k,r;
	const unsigned int size = 16;
//	cout << "Number: " << number << " Length: " << b.length() << endl;
//	cout << "=======" << endl;
	int seed = 79;//time(NULL);
	cout<<"Seed = "<<seed<<endl;
	Random::seed(seed);
	bool tst;
	unsigned int INT_MAX = -1;
	unsigned int n_tests = 5000000;

	Bignum<size> b;
	Bignum<size> bb;
	for(i=0; i<n_tests; i++)
	{
        /*
		if(!(i%500000))
			cout<<i<<endl;
		r = Random::random();
		b = i;
		bb = r;
		b += bb;
		k = b.to_uint();
		tst = (i+r == k);
//		cout<<"OKIS1" << endl;
		if(!tst)
		{
			cout << "Error3" << endl;
			cout << i << " " << r << " " << k << endl;
			cout << b << endl;
			cout << bb << endl;
			return 1;
		}
        */

		r = Random::random();
		b = i;
		bb = r;
		b -= bb;
		k = b.to_uint();
		tst = (i-r==k) || (r-i==k);
//		cout<<"OKIS3" << endl;
		if(!tst)
		{			
			cout << "Error5" << endl;
			cout << i << " " << r << " " << k << " " << i-r << " " << r-i << endl;
			cout << b << endl;
			cout << bb << endl;
			return 1;
		}

        /*
		if(i>0)
			r = Random::random()%((INT_MAX)/i);		
		b = i;
		bb = r;

		b *= bb;
		k = b.to_uint();
		tst = (i*r==k);
//		cout<<"OKIS5" << endl;
		if(!tst)
		{			
			cout << "Error7" << endl;
			cout << i << " " << r << " " << k << " " << i*r << endl;
			cout << b << endl;
			cout << b.to_uint() << endl;
			b = i*r;
			cout << b << endl;
			cout << b.to_uint() << endl;
			cout << bb << endl;
			cout << bb.to_uint() << endl;
			return 1;
		}
        */

        /*
		r = Random::random()%(INT_MAX-i)+i;
		b = i;
		bb = r;
		b /= bb;
		k = b.to_uint();
		tst = (i/r==k);
//		cout<<"OKIS7" << endl;
		if(!tst)
		{			
			cout << "Error9" << endl;
			cout << i << " " << r << " " << k << endl;
			cout << b << endl;
			cout << bb << endl;
			cout << i/r << endl;
			return 1;
		}

		r = Random::random();
		if(r==0) r++;
		b = i;
		bb = r;
		b %= bb;
		k = b.to_uint();
		tst = ((i%r)==k);
//		cout<<"OKIS9" << endl;
		if(!tst)
		{			
			cout << "Error11" << endl;
			cout << i << " " << r << " " << k << endl;
			cout << b << endl;
			cout << bb << endl;
			cout << i%r << endl;
			return 1;
		}
        */
	//	cout << b << endl;
	//	cout << i << " + " << j << " == " << (int)k << endl;
	}
//	cout << "Add ok\n\n" << endl;
	return 0;
}
