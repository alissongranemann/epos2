// EPOS Alarm Component Test Program

#include <utility/ostream.h>
#include <alarm.h>

using namespace EPOS;

const int iterations = 10;

int func_a(void);
int func_b(void);

Thread * a;
Thread * b;
Semaphore * sem_a;
Semaphore * sem_b;

OStream cout;

int main()
{
    cout << "Alarm test - Version 2 (using Semaphore_Handler)" << endl;

    cout << "I'm the first thread of the first task created in the system." << endl;
    cout << "I'll now create two alarms and put myself in a delay ..." << endl;

    sem_a = new Semaphore();
    sem_b = new Semaphore();
    a = new Thread(&func_a);
    b = new Thread(&func_b);

    Semaphore_Handler handler_a(sem_a);
    Alarm alarm_a(2000000, &handler_a, iterations);

    Semaphore_Handler handler_b(sem_b);
    Alarm alarm_b(1000000, &handler_b, iterations);

    Alarm::delay(2000000 * (iterations + 2));

    int status_a = a->join();
    int status_b = b->join();

    cout << "Thread A exited with status " << status_a
         << " and thread B exited with status " << status_b << "" << endl;

    delete a;
    delete b;
    delete sem_a;
    delete sem_b;

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
        sem_a->p();
        for(int i = 0; i < 79; i++)
            cout << "a";
        cout << endl;
    }

    return 'A';
}

int func_b(void)
{
    for(int it = iterations; it > 0; it--) {
        sem_b->p();
        for(int i = 0; i < 79; i++)
            cout << "b";
        cout << endl;
    }

    return 'B';
}
