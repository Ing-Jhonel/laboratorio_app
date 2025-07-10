#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
using namespace std;

class Recurso {
private:
    mutex mtx;
    condition_variable cv_mutex, cv_writer;
    int lec = 0;
    bool writing = false;

public:
    void lector() {
        while (true) {
            unique_lock<mutex> lock(mtx);
            while (writing) {
                cv_mutex.wait(lock);
            }
            lec++;
            if (lec == 1) {
                cv_writer.wait(lock);
            }
            lock.unlock();

            cout << "Leer del recurso" << endl;

            this_thread::sleep_for(chrono::milliseconds(100));

            lock.lock();
            lec--;
            if (lec == 0) {
                cv_writer.notify_one();
            }
            lock.unlock();
        }
    }

    void escritor() {
        while (true) {
            unique_lock<mutex> lock(mtx);
            while (lec > 0 || writing) {
                cv_writer.wait(lock);
            }
            writing = true;
            lock.unlock();

            cout << "Escribir recurso" << endl;

            this_thread::sleep_for(chrono::milliseconds(100));

            lock.lock();
            writing = false;
            cv_mutex.notify_all();
            cv_writer.notify_one();
            lock.unlock();
        }
    }
};

int main() {
    Recurso recurso;

    thread le1(&Recurso::lector, &recurso);
    thread le2(&Recurso::lector, &recurso);
    thread le3(&Recurso::lector, &recurso);

    thread es1(&Recurso::escritor, &recurso);
    thread es2(&Recurso::escritor, &recurso);
    thread es3(&Recurso::escritor, &recurso);

    le1.join();
    le2.join();
    le3.join();
    es1.join();
    es2.join();
    es3.join();

    return 0;
}