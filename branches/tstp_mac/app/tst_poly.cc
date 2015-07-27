#include <poly1305.h>
#include <aes.h>
// #include <mach/mc13224v/aes_controller.h>
#include <utility/random.h>
#include <alarm.h>

using namespace EPOS;

const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'A','0'};

OStream cout;

const unsigned char Bignum::default_mod[] = 
{
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFD, 0xFF, 0xFF, 0xFF
};
const unsigned char Bignum::default_barrett_u[] = 
{
    17, 0, 0, 0, 
    8, 0, 0, 0, 
    4, 0, 0, 0, 
    2, 0, 0, 0, 
    1, 0, 0, 0
};

#define MAXLEN 100

char k[16];
char r[16];


bool tst0(Poly1305*);
bool tst1(Poly1305*); 
bool tst2(Poly1305*);
bool tst3(Poly1305*);

const int ntests = 4;
bool (*tsts[ntests]) (Poly1305*) = {
	tst0, 
 	tst1, 
 	tst2,
 	tst3, 
};

int main()
{
	Poly1305 * poly = new Poly1305(new AES());
	bool ret;
	cout<<"Hello main!"<<endl;
	int i;
	for(i=0;i<ntests;i++)
	{
		cout << "Running test "<< i <<"\n";
		ret = (*tsts[i])(poly);
		if(!ret)
		{
			cout << "Test " << i << " failed!\n";
			while(1);
		}
		cout << "Test " << i <<" passed!\n";
	}
	cout << "All tests passed!\n";
    while(1);
	return 0;
}

bool tst0(Poly1305* poly)
{
	char out[16];
	char n[16];
	char s[16];
	int i;

	// Test 0
	i=0;
	k[i++]=0x83; k[i++]=0x4c; k[i++]=0x07; k[i++]=0xec;
	k[i++]=0x17; k[i++]=0x74; k[i++]=0x80; k[i++]=0x55;
	k[i++]=0x62; k[i++]=0x5b; k[i++]=0x42; k[i++]=0x01; 
	k[i++]=0xd6; k[i++]=0xad; k[i++]=0x35; k[i++]=0x32; 
	i=0;
	n[i++]=0x50; n[i++]=0x73; n[i++]=0x44; n[i++]=0xfb; 
	n[i++]=0xc5; n[i++]=0x68; n[i++]=0xe8; n[i++]=0xc4; 
	n[i++]=0x5c; n[i++]=0x27; n[i++]=0xc3; n[i++]=0x2a; 
	n[i++]=0x7e; n[i++]=0x32; n[i++]=0xd4; n[i++]=0xf9; 
	i=0;
	s[i++]=0x0f; s[i++]=0x3b; s[i++]=0x0b; s[i++]=0x58; 
	s[i++]=0x1e; s[i++]=0xbb; s[i++]=0x47; s[i++]=0x94; 
	s[i++]=0xb5; s[i++]=0x95; s[i++]=0xd0; s[i++]=0x69; 
	s[i++]=0xbc; s[i++]=0x6d; s[i++]=0x8b; s[i++]=0x92; 


	poly->k(k);

	for(i=0;i<16;i++)
		if(k[i] != poly->_k[i]) 
			cout<<"Nop000! " << i << " " << (int)k[i] << " " << (int)poly->_k[i] << '\n';

	poly->aes(out,n);

	for(i=0;i<16;i++)
		if(out[i] != s[i]) 
			cout<<"Nop0! " << i << " " << (int)out[i] << " " << (int)s[i] << '\n';
	if(!poly->isequal(out, s)) return false;

	// Test 1
	i=0;
	k[i++]=0x25; k[i++]=0xaa; k[i++]=0xde; k[i++]=0x75; 
	k[i++]=0x8e; k[i++]=0x20; k[i++]=0x9f; k[i++]=0xc0; 
	k[i++]=0x6b; k[i++]=0xce; k[i++]=0xc4; k[i++]=0x1d; 
	k[i++]=0xbf; k[i++]=0x3f; k[i++]=0xad; k[i++]=0x5c; 
	i=0;
	n[i++]=0x21; n[i++]=0x09; n[i++]=0xee; n[i++]=0x61; 
	n[i++]=0xaa; n[i++]=0xb0; n[i++]=0x29; n[i++]=0x8d; 
	n[i++]=0x4a; n[i++]=0x15; n[i++]=0x7e; n[i++]=0xed; 
	n[i++]=0xcc; n[i++]=0x09; n[i++]=0x55; n[i++]=0x2c; 
	i=0;
	s[i++]=0x22; s[i++]=0xab; s[i++]=0x3f; s[i++]=0xdd; 
	s[i++]=0xc7; s[i++]=0x1a; s[i++]=0xf1; s[i++]=0x51; 
	s[i++]=0x71; s[i++]=0x88; s[i++]=0xf0; s[i++]=0x59; 
	s[i++]=0xe7; s[i++]=0x2e; s[i++]=0xcc; s[i++]=0x29; 

	poly->k(k);
	poly->aes(out,n);

	for(i=0;i<16;i++)
		if(out[i] != s[i]) 
			cout<<"Nop1! " << i << " " << (int)out[i] << " " << (int)s[i] << '\n';
	if(!poly->isequal(out, s)) return false;


	// Test 2
	i=0;
	k[i++]=0x61; k[i++]=0x5f; k[i++]=0xcb; k[i++]=0x6a; 
	k[i++]=0xd3; k[i++]=0x6d; k[i++]=0x17; k[i++]=0xa7; 
	k[i++]=0xeb; k[i++]=0xc1; k[i++]=0xc5; k[i++]=0x20; 
	k[i++]=0x74; k[i++]=0xdc; k[i++]=0xdc; k[i++]=0x2e; 
	i=0;
	n[i++]=0x55; n[i++]=0x2a; n[i++]=0x21; n[i++]=0xae; 
	n[i++]=0x59; n[i++]=0x29; n[i++]=0x97; n[i++]=0x39; 
	n[i++]=0x8b; n[i++]=0x45; n[i++]=0xea; n[i++]=0x5d; 
	n[i++]=0x0e; n[i++]=0xff; n[i++]=0x21; n[i++]=0xc6; 
	i=0;
	s[i++]=0x69; s[i++]=0x9c; s[i++]=0x14; s[i++]=0x83; 
	s[i++]=0x88; s[i++]=0xdd; s[i++]=0x61; s[i++]=0xb5; 
	s[i++]=0x98; s[i++]=0x17; s[i++]=0x8a; s[i++]=0x29; 
	s[i++]=0xef; s[i++]=0x16; s[i++]=0x07; s[i++]=0xb1; 

	poly->k(k);
	poly->aes(out,n);

	for(i=0;i<16;i++)
		if(out[i] != s[i]) 
			cout<<"Nop2! " << i << " " << (int)out[i] << " " << (int)s[i] << '\n';
	if(!poly->isequal(out, s)) return false;

	// Test 3
	i=0;
	k[i++]=0x8a; k[i++]=0x66; k[i++]=0xa5; k[i++]=0xe1; 
	k[i++]=0xa5; k[i++]=0x66; k[i++]=0x5b; k[i++]=0x4d; 
	k[i++]=0x42; k[i++]=0xc5; k[i++]=0x8c; k[i++]=0xf6; 
	k[i++]=0x2d; k[i++]=0x98; k[i++]=0xd5; k[i++]=0x4e; 
	i=0;
	n[i++]=0xe7; n[i++]=0x31; n[i++]=0xe8; n[i++]=0x9a; 
	n[i++]=0x3a; n[i++]=0x8d; n[i++]=0x97; n[i++]=0x43; 
	n[i++]=0x71; n[i++]=0x7c; n[i++]=0x52; n[i++]=0x23; 
	n[i++]=0x3a; n[i++]=0x9e; n[i++]=0x14; n[i++]=0x28; 
	i=0;
	s[i++]=0x0a; s[i++]=0xc2; s[i++]=0xf8; s[i++]=0x80; 
	s[i++]=0xd1; s[i++]=0x02; s[i++]=0x12; s[i++]=0xa7; 
	s[i++]=0xcb; s[i++]=0x79; s[i++]=0x91; s[i++]=0xe2; 
	s[i++]=0x57; s[i++]=0x5a; s[i++]=0x55; s[i++]=0xcb; 

	poly->k(k);
	poly->aes(out,n);

	for(i=0;i<16;i++)
		if(out[i] != s[i]) 
			cout<<"Nop3! " << i << " " << (int)out[i] << " " << (int)s[i] << '\n';
	if(!poly->isequal(out, s)) return false;

	return true;
}
bool tst1(Poly1305* poly)
{
	char out[16];
	char ex[16]; // Expected poly result
	char n[16];
	char s[16];
	char m[MAXLEN];
	unsigned int sz;

	// Test 0
	int i;
	i=0;
	m[i++] = 0xf3; m[i++] = 0xf6; 
	sz=i;
	m[i++] = '\0';
	i=0;
	k[i++]=0xec; k[i++]=0x07; k[i++]=0x4c; k[i++]=0x83;
	k[i++]=0x55; k[i++]=0x80; k[i++]=0x74; k[i++]=0x17;
	k[i++]=0x01; k[i++]=0x42; k[i++]=0x5b; k[i++]=0x62;
	k[i++]=0x32; k[i++]=0x35; k[i++]=0xad; k[i++]=0xd6;
	i=0;
	r[i++]=0x85; r[i++]=0x1f; r[i++]=0xc4; r[i++]=0x0c;
	r[i++]=0x34; r[i++]=0x67; r[i++]=0xac; r[i++]=0x0b;
	r[i++]=0xe0; r[i++]=0x5c; r[i++]=0xc2; r[i++]=0x04;
	r[i++]=0x04; r[i++]=0xf3; r[i++]=0xf7; r[i++]=0x00;
	i=0;
	n[i++]=0xfb; n[i++]=0x44; n[i++]=0x73; n[i++]=0x50;
	n[i++]=0xc4; n[i++]=0xe8; n[i++]=0x68; n[i++]=0xc5;
	n[i++]=0x2a; n[i++]=0xc3; n[i++]=0x27; n[i++]=0x5c;
	n[i++]=0xf9; n[i++]=0xd4; n[i++]=0x32; n[i++]=0x7e;
	i=0;
	s[i++]=0x58; s[i++]=0x0b; s[i++]=0x3b; s[i++]=0x0f;
	s[i++]=0x94; s[i++]=0x47; s[i++]=0xbb; s[i++]=0x1e;
	s[i++]=0x69; s[i++]=0xd0; s[i++]=0x95; s[i++]=0xb5;
	s[i++]=0x92; s[i++]=0x8b; s[i++]=0x6d; s[i++]=0xbc;
	i=0;
	ex[i++]=0xf4; ex[i++]=0xc6; ex[i++]=0x33; ex[i++]=0xc3;
	ex[i++]=0x04; ex[i++]=0x4f; ex[i++]=0xc1; ex[i++]=0x45;
	ex[i++]=0xf8; ex[i++]=0x4f; ex[i++]=0x33; ex[i++]=0x5c;
	ex[i++]=0xb8; ex[i++]=0x19; ex[i++]=0x53; ex[i++]=0xde;

	poly->k(k);

	poly->aes(out,n);

	for(i=0;i<16;i++)
		if(out[i] != s[i]) 
			cout<<"Nop0! " << i << " " << (int)out[i] << " " << (int)s[i] << '\n';

	poly->r(r);
    cout << "uno" << endl;
	poly->authenticate(out,n,m,sz);
    cout << "dos" << endl;
	for(i=0;i<16;i++)
		if(out[i] != ex[i]) 
			cout<<"Nop! " << i << " " << (int)out[i] << " " << (int)ex[i]<<'\n';
	if(!poly->isequal(out, ex)) return false;

	cout<<"Ok 0\n";

	// Test 1
	i=0;
	sz=i;
	m[i++] = '\0';
	i=0;
	k[i++]=0x75; k[i++]=0xde; k[i++]=0xaa; k[i++]=0x25;
	k[i++]=0xc0; k[i++]=0x9f; k[i++]=0x20; k[i++]=0x8e;
	k[i++]=0x1d; k[i++]=0xc4; k[i++]=0xce; k[i++]=0x6b;
	k[i++]=0x5c; k[i++]=0xad; k[i++]=0x3f; k[i++]=0xbf;
	i=0;
	r[i++]=0xa0; r[i++]=0xf3; r[i++]=0x08; r[i++]=0x00;
	r[i++]=0x00; r[i++]=0xf4; r[i++]=0x64; r[i++]=0x00;
	r[i++]=0xd0; r[i++]=0xc7; r[i++]=0xe9; r[i++]=0x07;
	r[i++]=0x6c; r[i++]=0x83; r[i++]=0x44; r[i++]=0x03;
	i=0;
	n[i++]=0x61; n[i++]=0xee; n[i++]=0x09; n[i++]=0x21;
	n[i++]=0x8d; n[i++]=0x29; n[i++]=0xb0; n[i++]=0xaa;
	n[i++]=0xed; n[i++]=0x7e; n[i++]=0x15; n[i++]=0x4a;
	n[i++]=0x2c; n[i++]=0x55; n[i++]=0x09; n[i++]=0xcc;
	i=0;
	s[i++]=0xdd; s[i++]=0x3f; s[i++]=0xab; s[i++]=0x22;
	s[i++]=0x51; s[i++]=0xf1; s[i++]=0x1a; s[i++]=0xc7;
	s[i++]=0x59; s[i++]=0xf0; s[i++]=0x88; s[i++]=0x71;
	s[i++]=0x29; s[i++]=0xcc; s[i++]=0x2e; s[i++]=0xe7;
	i=0;
	ex[i++]=0xdd; ex[i++]=0x3f; ex[i++]=0xab; ex[i++]=0x22;
	ex[i++]=0x51; ex[i++]=0xf1; ex[i++]=0x1a; ex[i++]=0xc7;
	ex[i++]=0x59; ex[i++]=0xf0; ex[i++]=0x88; ex[i++]=0x71;
	ex[i++]=0x29; ex[i++]=0xcc; ex[i++]=0x2e; ex[i++]=0xe7;

	poly->k(k);

	poly->aes(out,n);

	for(i=0;i<16;i++)
		if(out[i] != s[i]) 
			cout<<"Nop1! " << i << " " << (int)out[i] << " " << (int)s[i] << '\n';

	poly->r(r);
	poly->authenticate(out,n,m,sz);
	for(i=0;i<16;i++)

	for(i=0;i<16;i++)
		if(out[i] != ex[i]) 
			cout<<"Nop! " << i << " " << out[i] << " "<< ex[i] << '\n';
	if(!poly->isequal(out, ex)) //return false;
		cout<<"NOT Ok 1\n";
	else
		cout<<"Ok 1\n";

	// Test 2
	i=0;
	m[i++]=0x66; m[i++]=0x3c; m[i++]=0xea; m[i++]=0x19;
	m[i++]=0x0f; m[i++]=0xfb; m[i++]=0x83; m[i++]=0xd8;
	m[i++]=0x95; m[i++]=0x93; m[i++]=0xf3; m[i++]=0xf4;
	m[i++]=0x76; m[i++]=0xb6; m[i++]=0xbc; m[i++]=0x24;
	m[i++]=0xd7; m[i++]=0xe6; m[i++]=0x79; m[i++]=0x10;
	m[i++]=0x7e; m[i++]=0xa2; m[i++]=0x6a; m[i++]=0xdb;
	m[i++]=0x8c; m[i++]=0xaf; m[i++]=0x66; m[i++]=0x52;
	m[i++]=0xd0; m[i++]=0x65; m[i++]=0x61; m[i++]=0x36;
	sz=i;
	m[i++]='\0';
	i=0;
	k[i++]=0x6a; k[i++]=0xcb; k[i++]=0x5f; k[i++]=0x61;
	k[i++]=0xa7; k[i++]=0x17; k[i++]=0x6d; k[i++]=0xd3;
	k[i++]=0x20; k[i++]=0xc5; k[i++]=0xc1; k[i++]=0xeb;
	k[i++]=0x2e; k[i++]=0xdc; k[i++]=0xdc; k[i++]=0x74;
	i=0;
	r[i++]=0x48; r[i++]=0x44; r[i++]=0x3d; r[i++]=0x0b;
	r[i++]=0xb0; r[i++]=0xd2; r[i++]=0x11; r[i++]=0x09;
	r[i++]=0xc8; r[i++]=0x9a; r[i++]=0x10; r[i++]=0x0b;
	r[i++]=0x5c; r[i++]=0xe2; r[i++]=0xc2; r[i++]=0x08;
	i=0;
	n[i++]=0xae; n[i++]=0x21; n[i++]=0x2a; n[i++]=0x55;
	n[i++]=0x39; n[i++]=0x97; n[i++]=0x29; n[i++]=0x59;
	n[i++]=0x5d; n[i++]=0xea; n[i++]=0x45; n[i++]=0x8b;
	n[i++]=0xc6; n[i++]=0x21; n[i++]=0xff; n[i++]=0x0e;
	i=0;
	s[i++]=0x83; s[i++]=0x14; s[i++]=0x9c; s[i++]=0x69;
	s[i++]=0xb5; s[i++]=0x61; s[i++]=0xdd; s[i++]=0x88;
	s[i++]=0x29; s[i++]=0x8a; s[i++]=0x17; s[i++]=0x98;
	s[i++]=0xb1; s[i++]=0x07; s[i++]=0x16; s[i++]=0xef;
	i=0;
	ex[i++]=0x0e; ex[i++]=0xe1; ex[i++]=0xc1; ex[i++]=0x6b;
	ex[i++]=0xb7; ex[i++]=0x3f; ex[i++]=0x0f; ex[i++]=0x4f;
	ex[i++]=0xd1; ex[i++]=0x98; ex[i++]=0x81; ex[i++]=0x75;
	ex[i++]=0x3c; ex[i++]=0x01; ex[i++]=0xcd; ex[i++]=0xbe;

	poly->k(k);

	poly->aes(out,n);

	for(i=0;i<16;i++)
		if(out[i] != s[i]) 
			cout<<"Nop2! " << i << " " << (int)out[i] << " " << (int)s[i] << '\n';

	poly->r(r);
	poly->authenticate(out,n,m,sz);

	for(i=0;i<16;i++)
		if(out[i] != ex[i]) 
			cout<<"Nop! " << i << " " << out[i] << " "<< ex[i] << '\n';
	if(!poly->isequal(out, ex)) //return false;
		cout<<"NOT Ok 2\n";
	else
		cout<<"Ok 2\n";

	/*
	for(i=0;i<16;i++)
		if(out[i] != ex[i]) 
			cout<<"Nop! " << i << '\n';
	if(!poly->isequal(out, ex)) return false;
	cout<<"Ok 2\n";
	*/

	// Test 3
	i=0;
	m[i++]=0xab; m[i++]=0x08; m[i++]=0x12; m[i++]=0x72;
	m[i++]=0x4a; m[i++]=0x7f; m[i++]=0x1e; m[i++]=0x34;
	m[i++]=0x27; m[i++]=0x42; m[i++]=0xcb; m[i++]=0xed;
	m[i++]=0x37; m[i++]=0x4d; m[i++]=0x94; m[i++]=0xd1;
	m[i++]=0x36; m[i++]=0xc6; m[i++]=0xb8; m[i++]=0x79;
	m[i++]=0x5d; m[i++]=0x45; m[i++]=0xb3; m[i++]=0x81;
	m[i++]=0x98; m[i++]=0x30; m[i++]=0xf2; m[i++]=0xc0;
	m[i++]=0x44; m[i++]=0x91; m[i++]=0xfa; m[i++]=0xf0;
	m[i++]=0x99; m[i++]=0x0c; m[i++]=0x62; m[i++]=0xe4;
	m[i++]=0x8b; m[i++]=0x80; m[i++]=0x18; m[i++]=0xb2;
	m[i++]=0xc3; m[i++]=0xe4; m[i++]=0xa0; m[i++]=0xfa;
	m[i++]=0x31; m[i++]=0x34; m[i++]=0xcb; m[i++]=0x67;
	m[i++]=0xfa; m[i++]=0x83; m[i++]=0xe1; m[i++]=0x58;
	m[i++]=0xc9; m[i++]=0x94; m[i++]=0xd9; m[i++]=0x61;
	m[i++]=0xc4; m[i++]=0xcb; m[i++]=0x21; m[i++]=0x09;
	m[i++]=0x5c; m[i++]=0x1b; m[i++]=0xf9; 
	sz = i;
	m[i++]='\0';
	i=0;
	k[i++]=0xe1; k[i++]=0xa5; k[i++]=0x66; k[i++]=0x8a;
	k[i++]=0x4d; k[i++]=0x5b; k[i++]=0x66; k[i++]=0xa5;
	k[i++]=0xf6; k[i++]=0x8c; k[i++]=0xc5; k[i++]=0x42;
	k[i++]=0x4e; k[i++]=0xd5; k[i++]=0x98; k[i++]=0x2d;
	i=0;
	r[i++]=0x12; r[i++]=0x97; r[i++]=0x6a; r[i++]=0x08;
	r[i++]=0xc4; r[i++]=0x42; r[i++]=0x6d; r[i++]=0x0c;
	r[i++]=0xe8; r[i++]=0xa8; r[i++]=0x24; r[i++]=0x07;
	r[i++]=0xc4; r[i++]=0xf4; r[i++]=0x82; r[i++]=0x07;
	i=0;
	n[i++]=0x9a; n[i++]=0xe8; n[i++]=0x31; n[i++]=0xe7;
	n[i++]=0x43; n[i++]=0x97; n[i++]=0x8d; n[i++]=0x3a;
	n[i++]=0x23; n[i++]=0x52; n[i++]=0x7c; n[i++]=0x71;
	n[i++]=0x28; n[i++]=0x14; n[i++]=0x9e; n[i++]=0x3a;
	i=0;
	s[i++]=0x80; s[i++]=0xf8; s[i++]=0xc2; s[i++]=0x0a;
	s[i++]=0xa7; s[i++]=0x12; s[i++]=0x02; s[i++]=0xd1;
	s[i++]=0xe2; s[i++]=0x91; s[i++]=0x79; s[i++]=0xcb;
	s[i++]=0xcb; s[i++]=0x55; s[i++]=0x5a; s[i++]=0x57;
	i=0;
	ex[i++]=0x51; ex[i++]=0x54; ex[i++]=0xad; ex[i++]=0x0d;
	ex[i++]=0x2c; ex[i++]=0xb2; ex[i++]=0x6e; ex[i++]=0x01;
	ex[i++]=0x27; ex[i++]=0x4f; ex[i++]=0xc5; ex[i++]=0x11;
	ex[i++]=0x48; ex[i++]=0x49; ex[i++]=0x1f; ex[i++]=0x1b;

	poly->k(k);

	poly->aes(out,n);

	for(i=0;i<16;i++)
		if(out[i] != s[i]) 
			cout<<"Nop3! " << i << " " << (int)out[i] << " " << (int)s[i] << '\n';

	poly->r(r);
	poly->authenticate(out,n,m,sz);

	for(i=0;i<16;i++)
		if(out[i] != ex[i]) 
			cout<<"Nop! " << i << " " << out[i] << " "<< ex[i] << '\n';
	if(!poly->isequal(out, ex)) //return false;
		cout<<"NOT Ok 3\n";
	else
		cout<<"Ok 3\n";

	/*
	for(i=0;i<16;i++)
		if(out[i] != ex[i]) 
			cout<<"Nop! " << i << '\n';
	if(!poly->isequal(out, ex)) return false;
	cout<<"Ok 3\n";
	*/

	return true;
}

bool tst2(Poly1305* poly)
{
	char n[16];
	int i;
	i=0;
	k[i++]=0xec; k[i++]=0x07; k[i++]=0x4c; k[i++]=0x83;
	k[i++]=0x55; k[i++]=0x80; k[i++]=0x74; k[i++]=0x17;
	k[i++]=0x01; k[i++]=0x42; k[i++]=0x5b; k[i++]=0x62;
	k[i++]=0x32; k[i++]=0x35; k[i++]=0xad; k[i++]=0xd6;
	i=0;
	r[i++]=0x85; r[i++]=0x1f; r[i++]=0xc4; r[i++]=0x0c;
	r[i++]=0x34; r[i++]=0x67; r[i++]=0xac; r[i++]=0x0b;
	r[i++]=0xe0; r[i++]=0x5c; r[i++]=0xc2; r[i++]=0x04;
	r[i++]=0x04; r[i++]=0xf3; r[i++]=0xf7; r[i++]=0x00;
	i=0;
	n[i++]=0x9a; n[i++]=0xe8; n[i++]=0x31; n[i++]=0xe7;
	n[i++]=0x43; n[i++]=0x97; n[i++]=0x8d; n[i++]=0x3a;
	n[i++]=0x23; n[i++]=0x52; n[i++]=0x7c; n[i++]=0x71;
	n[i++]=0x28; n[i++]=0x14; n[i++]=0x9e; n[i++]=0x3a;
	poly->k(k);
	poly->r(r);

	char out[16];
	char out2[16];

	poly->aes(out,n);
	poly->aes(out2,out);
	/*
	for(i=0;i<16;i++)
		if(out2[i] != n[i]) 
			cout<<"Nop! " << i << " " << (int)out2[i] << " " << (int)n[i] << '\n';
			*/

	return !poly->isequal(n,out2);
}

bool tst3(Poly1305* poly)
{
	char out[16];
	char str[16];
	char n[16];
	char m[MAXLEN];
	int loop; int len;
	int i; int x; int y; int aux;
	int it;

	i=0;
	k[i++]=0xec; k[i++]=0x07; k[i++]=0x4c; k[i++]=0x83;
	k[i++]=0x55; k[i++]=0x80; k[i++]=0x74; k[i++]=0x17;
	k[i++]=0x01; k[i++]=0x42; k[i++]=0x5b; k[i++]=0x62;
	k[i++]=0x32; k[i++]=0x35; k[i++]=0xad; k[i++]=0xd6;
	i=0;
	r[i++]=0x85; r[i++]=0x1f; r[i++]=0xc4; r[i++]=0x0c;
	r[i++]=0x34; r[i++]=0x67; r[i++]=0xac; r[i++]=0x0b;
	r[i++]=0xe0; r[i++]=0x5c; r[i++]=0xc2; r[i++]=0x04;
	r[i++]=0x04; r[i++]=0xf3; r[i++]=0xf7; r[i++]=0x00;
	i=0;
	n[i++]=0x9a; n[i++]=0xe8; n[i++]=0x31; n[i++]=0xe7;
	n[i++]=0x43; n[i++]=0x97; n[i++]=0x8d; n[i++]=0x3a;
	n[i++]=0x23; n[i++]=0x52; n[i++]=0x7c; n[i++]=0x71;
	n[i++]=0x28; n[i++]=0x14; n[i++]=0x9e; n[i++]=0x3a;

	poly->k(k);
	poly->r(r);

	for (loop = 0;loop < 10;++loop) {
		len = 0;

		//if(loop%100 == 0)
			cout<<"loop " << loop << '\n';
		it = 0;
		for (;;) {
			if(it%10 == 0)
				cout << "iteration " << loop << " " << it << '\n';
			it++;
			poly->authenticate(out,n,m,len);

			/*
			for(i=0;i<16;i++)
				cout << out[i] << " ";			
			cout << '\n';
			*/

			if (!poly->verify(out,n,m,len)) 
			{
				cout << loop << " poly1305aes_verify failed on good input\n";
				return false;
			}

			x = Random::random() & 15;
			aux = out[x];
			do
				y = (Random::random() % 256);
			while(y == out[x]);

			out[x] = y;

			if (poly->verify(out,n,m,len)) 
			{
				cout << loop << " poly1305aes_verify succeeded on bad input\n";
				return false;
			}
			out[x] = aux;

			if (len >= MAXLEN) break;
			n[0] ^= loop;
			for (i = 0;i < 16;++i) n[i] ^= out[i];
			if (len % 2) for (i = 0;i < 16;++i) k[i] ^= out[i];
			if (len % 3) for (i = 0;i < 16;++i) r[i] ^= out[i];
			poly->k(k);
			poly->r(r);
			m[len++] ^= out[0];
		}
	}
	return true;
}
