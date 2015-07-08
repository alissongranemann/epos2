// EPOS TimeStampCounter Test Program

#include <utility/ostream.h>
#include <tsc.h>

using namespace EPOS;

int main()
{
    unsigned int t1, t2;
    OStream cout;

    cout << "Counting up to 10 seconds" << endl;
    do
    {
        t1 = TSC::time_stamp() / TSC::frequency();
        t2 = TSC::time_stamp() / TSC::frequency();
        while(t1 == t2)
            t2 = TSC::time_stamp() / TSC::frequency();
        cout << t2 << endl;
    } while(t2 < 10);

    cout << "Setting total offset of +100 seconds" << endl;
    TSC::offset(100*TSC::frequency(),true);
    cout << "Counting up to 120 seconds" << endl;
    do
    {
        t1 = TSC::time_stamp() / TSC::frequency();
        t2 = TSC::time_stamp() / TSC::frequency();
        while(t1 == t2)
            t2 = TSC::time_stamp() / TSC::frequency();
        cout << t2 << endl;
    } while(t2 < 120);

    cout << "Setting total offset of -20 seconds" << endl;
    TSC::offset(20*TSC::frequency(),false);
    cout << "Counting up to 10 seconds" << endl;
    do
    {
        t1 = TSC::time_stamp() / TSC::frequency();
        t2 = TSC::time_stamp() / TSC::frequency();
        while(t1 == t2)
            t2 = TSC::time_stamp() / TSC::frequency();
        cout << t2 << endl;
    } while(t2 < 10);

    cout << "Done!" << endl;
    return 0;
}
