// EPOS Cipher Mediator Test Program

#include <cipher.h>
#include <utility/ostream.h>
#include <utility/random.h>

using namespace EPOS;

OStream cout;

static const unsigned int SEED = 1337;
static const unsigned int ITERATIONS = 500;

int main()
{
    cout << "EPOS Cipher Test" << endl;
    cout << "Configuration: " << endl;
    cout << "Traits<Xor_Cipher>::enabled = " << Traits<Xor_Cipher>::enabled << endl;
    cout << "Cipher::KEY_SIZE = " << Cipher::KEY_SIZE << endl;
    cout << "Random seed = " << SEED << endl;
    cout << "Iterations = " << ITERATIONS << endl;

    unsigned int tests_failed = 0;

    Random::seed(SEED);

    for(unsigned int it = 0; it < ITERATIONS; it++) {
        char clear_text[Cipher::KEY_SIZE];
        char cipher_text[Cipher::KEY_SIZE];
        char decrypted_text[Cipher::KEY_SIZE];
        char key[Cipher::KEY_SIZE];
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++) {
            clear_text[i] = Random::random();
            key[i] = Random::random();
        }

        cout << endl;
        cout << "Iteration " << it << endl;

        cout << "clear_text =";
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(clear_text[i]);
        cout << endl;

        cout << "key =";
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(key[i]);
        cout << endl;

        cout << endl;
        cout << "Testing encryption..." << endl;
        Cipher::encrypt(clear_text, key, cipher_text);

        cout << "cipher_text =";
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(cipher_text[i]);
        cout << endl;

        bool ok = false;
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++)
            if(decrypted_text[i] != clear_text[i]) {
                ok = true;
                break;
            }
        if(ok)
            cout << "OK! cipher_text does not match clear_text" << endl;
        else
            cout << "ERROR! cipher_text matches clear_text!" << endl;
        tests_failed += !ok;


        cout << endl;
        cout << "Testing decryption..." << endl;

        Cipher::decrypt(cipher_text, key, decrypted_text);

        cout << "decrypted_text =";
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(decrypted_text[i]);
        cout << endl;

        ok = true;
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++)
            if(decrypted_text[i] != clear_text[i]) {
                ok = false;
                break;
            }
        if(ok)
            cout << "OK! decrypted_text matches clear_text" << endl;
        else
            cout << "ERROR! decrypted_text does not match clear_text!" << endl;
        tests_failed += !ok;


        cout << endl;
        cout << "Testing decryption with wrong key..." << endl;

        key[Random::random() % Cipher::KEY_SIZE]++;
        cout << "key =";
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(key[i]);
        cout << endl;

        Cipher::decrypt(cipher_text, key, decrypted_text);

        cout << "decrypted_text =";
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(decrypted_text[i]);
        cout << endl;

        ok = false;
        for(unsigned int i = 0; i < Cipher::KEY_SIZE; i++)
            if(decrypted_text[i] != clear_text[i]) {
                ok = true;
                break;
            }
        if(ok)
            cout << "OK! decrypted_text does not match clear_text" << endl;
        else
            cout << "ERROR! decrypted_text with wrong key matches clear_text!" << endl;
        tests_failed += !ok;
    }

    cout << endl;
    cout << "Tests finished with " << tests_failed << " error" << (tests_failed > 1 ? "s" : "") << " detected." << endl;
    cout << endl;

    return 0;
}
