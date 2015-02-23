// EPOS Tests for Metaprograms

#include <utility/ostream.h>
#include <system/meta.h>

using namespace EPOS;

// typedef _SYS::DESERIALIZE DESERIALIZE;
// typedef _SYS::SERIALIZE SERIALIZE;

static const unsigned int MAX_BUFFER_SIZE = 20;
OStream cout;

class Alpha
{
public:
    Alpha()
    {
        _a = 0;
        _b = '\0';
        _c = 0.0;
        _d = 0;
    }
    
    Alpha(int a, char b, float c, int d)
    {
        _a = a; 
        _b = b;
        _c = c;
        _d = d;
    }


    friend OStream & operator<<(OStream & cout, const Alpha & alpha) {
        cout << "={" << alpha._a << "," << alpha._b << "," << alpha._c << "," << alpha._d << "}";
        return cout;
    }


    bool operator==(const Alpha & other) const 
    {
        return (_a == other._a) 
            && (_b == other._b) 
            && (_c == other._c)
            && (_d == other._d);
    }

    
    bool operator!=(const Alpha & other) const 
    {
        return !(*this == other);
    }
                
        
public:
    int _a;
    char _b;
    float _c;
    int _d;
};


void clear_buffer(char * buffer)
{
    for(unsigned int i = 0; i < MAX_BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
}


void test_serialize_deserialize_one_parameter()
{
    cout << "test_serialize_deserialize_one_parameter" << endl;

    char buffer[MAX_BUFFER_SIZE];

    int x;
    _SYS::SERIALIZE(buffer, 0, 28);
    _SYS::DESERIALIZE(buffer, 0, x);

    if (x != 28) {
        cout << "Objects differ" << endl;
        cout << "x: " << x << endl;
        cout << "should be:  " << 28 << endl;

        cout << "fail!" << endl;
        return;
    }

    clear_buffer(buffer);
    _SYS::DESERIALIZE(buffer, 0, x);
    if (x == 28) {
        cout << "Objects are equal and they shouldn't" << endl;

        cout << "fail!" << endl;
        return;
    }    

    cout << "pass" << endl;
}


void test_serialize_deserialize_four_parameters()
{
    cout << "test_serialize_deserialize_four_parameters" << endl;

    char buffer[MAX_BUFFER_SIZE];
    Alpha * alpha = new Alpha(7, 'M', 3.14, -8);
    Alpha * beta = new Alpha();

    _SYS::SERIALIZE(buffer, 0, alpha->_a, alpha->_b, alpha->_c, alpha->_d);
    _SYS::DESERIALIZE(buffer, 0, beta->_a, beta->_b, beta->_c, beta->_d);

    if (*alpha != *beta) {
        cout << "Objects differ" << endl;
        cout << "alpha: " << *alpha << endl;
        cout << "beta:  " << *beta << endl;
        cout << "buffer: {";
        for(unsigned int i = 0; i < MAX_BUFFER_SIZE; i++) {
            cout << (void *) buffer[i] << ",";
        }
        cout << "}" << endl;

        cout << "fail!" << endl;
        return;
    }

    clear_buffer(buffer);
    _SYS::DESERIALIZE(buffer, 0, beta->_a, beta->_b, beta->_c, beta->_d);
    if (*alpha == *beta) {
        cout << "Objects are equal and they shouldn't" << endl;

        cout << "fail!" << endl;
        return;
    }    

    delete alpha;
    delete beta;

    cout << "pass" << endl;
}


void test_serialize_deserialize()
{
    test_serialize_deserialize_four_parameters();
    test_serialize_deserialize_one_parameter();
}


int main()
{
    cout << "Meta test" << endl;
    
    test_serialize_deserialize();
    
    cout << "The End!" << endl;

    return 0;
}
