// EPOS Alarm Component Test Program

#include <utility/ostream.h>
#include <alarm.h>
#include <condition.h>

using namespace EPOS;

const int iterations = 10;

int func_a(void);
int func_b(void);

Thread * a;
Thread * b;
Condition * cond_a;
Condition * cond_b;

OStream cout;

int main()
{
    cout << "Alarm test - Version 4 (using Condition_Handler)" << endl;

    cout << "I'm the first thread of the first task created in the system." << endl;
    cout << "I'll now create two alarms and put myself in a delay ..." << endl;

    cond_a = new Condition();
    cond_b = new Condition();
    a = new Thread(&func_a);
    b = new Thread(&func_b);

    Condition_Handler handler_a(cond_a);
    Alarm alarm_a(2000000, &handler_a, iterations);

    Condition_Handler handler_b(cond_b);
    Alarm alarm_b(1000000, &handler_b, iterations);

    Alarm::delay(2000000 * (iterations + 2));

    int status_a = a->join();
    int status_b = b->join();

    cout << "Thread A exited with status " << status_a
         << " and thread B exited with status " << status_b << "" << endl;

    delete a;
    delete b;
    delete cond_a;
    delete cond_b;

    cout << "Some info about the alarms used in this application" << endl;
    cout << "alarm_a period = " << alarm_a.period() << " us" << endl;
    cout << "alarm_b period = " << alarm_b.period() << " us" << endl;
    cout << "Alarm timer frequency = " << Alarm::frequency() << " Hertz" << endl;

    cout << "I'm done, bye!" << endl;

    return 0;
}

int func_a()
{
    for(int it = iterations; it > 0; it--) {
        cond_a->wait();
        for(int i = 0; i < 79; i++)
            cout << "a";
        cout << endl;
    }

    return 'A';
}

int func_b(void)
{
    for(int it = iterations; it > 0; it--) {
        cond_b->wait();
        for(int i = 0; i < 79; i++)
            cout << "b";
        cout << endl;
    }

    return 'B';
}
