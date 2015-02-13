#include <utility/ostream.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

int print(const char * string)
{
    cout << string << endl;
    return 0;
}

int main()
{
    const char string[] = "Hello, I am the main thread, and I am printing this long sentence";
    cout << string << endl;
    cout << string << endl;
    /*
    Thread *a;
    while(1)
    {
        print(string);
        print(string);
        Alarm::delay(1000000);
        a = new Thread(&print, "thread");
        for(int i=0;i<10;i++)
        {
            print(string);
            Alarm::delay(1000000);
        }
    }
    */
    
    return 0;
}
