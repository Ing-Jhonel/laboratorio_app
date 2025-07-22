#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

mutex mtx;
condition_variable cv_lector, cv_escritor;
int lectores = 0;
bool escribiendo = false;
queue<string> mensajes;

void lector(int id) {
    while(true) {
        unique_lock<mutex> lock(mtx);
        cv_lector.wait(lock, [] { return !escribiendo; });
        lectores++;
        if(lectores == 1) {
            cv_escritor.notify_one();
        }
        lock.unlock();

        if(!mensajes.empty()) {
            string msg = mensajes.front();
            cout << "Lector " << id << " lee: " << msg << endl;
        }

        lock.lock();
        lectores--;
        if(lectores == 0) {
            cv_escritor.notify_one();
        }
        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void escritor(int id) {
    while(true) {
        unique_lock<mutex> lock(mtx);
        cv_escritor.wait(lock, [] { return lectores == 0 && !escribiendo; });
        escribiendo = true;
        lock.unlock();

        string msg = "Mensaje del escritor " + to_string(id);
        mensajes.push(msg);
        cout << "Escritor " << id << " escribe: " << msg << endl;

        lock.lock();
        escribiendo = false;
        cv_lector.notify_all();
        cv_escritor.notify_one();
        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {
    thread l1(lector, 1);
    thread l2(lector, 2);
    thread l3(lector, 3);

    thread e1(escritor, 1);
    thread e2(escritor, 2);
    thread e3(escritor, 3);

    l1.join();
    l2.join();
    l3.join();
    e1.join();
    e2.join();
    e3.join();

    return 0;
}
