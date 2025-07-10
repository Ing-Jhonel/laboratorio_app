#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <random>
#include <chrono>

const int N = 5;
using namespace std;

class Palillos {
public:
    Palillos() {
        for (int i = 0; i < N; ++i) {
            palillos[i] = true;
        }
    }

    bool tomar(int i) {
        unique_lock<mutex> lock(mtx);
        while (!palillos[i]) {
            cv.wait(lock);
        }
        palillos[i] = false;
        return true;
    }

    void soltar(int i) {
        unique_lock<mutex> lock(mtx);
        palillos[i] = true;
        cv.notify_all();
    }

private:
    bool palillos[N];
    mutex mtx;
    condition_variable cv;
};

Palillos palillos;

void filosofos(int i) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1000, 2000);

    while (true) {
        cout << "Filosofo " << i << " esta pensando." << endl;
        this_thread::sleep_for(chrono::milliseconds(dis(gen)));

        cout << "Filosofo " << i << " quiere tomar los palillos." << endl;

        if (i % 2 == 0) {
            palillos.tomar(i);
            palillos.tomar((i + 1) % N);
        } else {
            palillos.tomar((i + 1) % N);
            palillos.tomar(i);
        }

        cout << "Filosofo " << i << " esta comiendo." << endl;
        this_thread::sleep_for(chrono::milliseconds(dis(gen)));

        palillos.soltar(i);
        palillos.soltar((i + 1) % N);

        cout << "Filosofo " << i << " ha terminado de comer y esta pensando nuevamente." << endl;
    }
}

int main() {
    thread filosofos_threads[N];
    
    for (int i = 0; i < N; ++i) {
        filosofos_threads[i] = thread(filosofos, i);
    }

    for (int i = 0; i < N; ++i) {
        filosofos_threads[i].join();
    }

    return 0;
}