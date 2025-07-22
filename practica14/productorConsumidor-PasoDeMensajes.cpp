#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

mutex mtx;
condition_variable cv_prod, cv_cons;
queue<int> mensajes;
bool producido = false;
bool consumido = true;
const int N = 5;

void productor() {
    for (int i = 1; i <= N; ++i) {
        unique_lock<mutex> lock(mtx);
        cv_prod.wait(lock, [] { return consumido; });
        mensajes.push(i);
        cout << "Productor envio mensaje: " << i << endl;
        producido = true;
        consumido = false;
        lock.unlock();
        cv_cons.notify_one();
    }
}

void consumidor() {
    for (int i = 1; i <= N; ++i) {
        unique_lock<mutex> lock(mtx);
        cv_cons.wait(lock, [] { return producido; });
        int msg = mensajes.front();
        mensajes.pop();
        cout << "Consumidor recibio mensaje: " << msg << endl;
        producido = false;
        consumido = true;
        lock.unlock();
        cv_prod.notify_one();
    }
}

int main() {
    thread prod(productor);
    thread cons(consumidor);

    prod.join();
    cons.join();

    return 0;
}
