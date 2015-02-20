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
    register unsigned int a,b;
    ASM("mov %0, sp\n":"=r"(a));
    UART u;
    const char string[] = "Hello, I am the main thread, and I am printing this long sentence\r\n";
    for(auto j=0;j<5;j++)
    for(auto i=0;string[i]!='\0';i++)
    {
//         Alarm::delay(500);
        u.put(string[i]);
//         cout << i;
    }
//     cout <<"huhuhuhuhuhuhuhuhuhuhuhuhuhuhhu" << endl;
//     ASM("svc #1");
//      ASM("CHARLIEUE:\n"
//          "ldr r0, CHARLIEUE\n"
//              "blx r0\n");
//     for(volatile int i=0;i<0xffffff;i++);
    /*
    UART u;
    const char string[] = "Hello, I am the main thread, and I am printing this long sentence\r\n";
    for(auto j=0;j<5;j++)
    for(auto i=0;string[i]!='\0';i++)
    {
//         Alarm::delay(500);
        u.put(string[i]);
//         cout << i;
    }
//     cout << string << endl;
//     cout << string << endl;
    while(1);
    */
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
