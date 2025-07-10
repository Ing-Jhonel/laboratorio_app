#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <random>
#include <chrono>

const int N = 5;
using namespace std;

class Barberia {
public:
    Barberia() : sillas_libres(N), cliente_listo(0), cliente_sentado(0) {}

    void cortarPelo(int i) {
        unique_lock<mutex> lock(mtx);
        while (cliente_listo == 0) {
            cout << "Barbero duerme." << endl;
            cv_barbero.wait(lock);
        }
        cout << "Barbero esta cortando el pelo al cliente " << i << endl;
        cliente_listo--;
        cliente_sentado++;
    }

    void esperarTurno(int i) {
        unique_lock<mutex> lock(mtx);
        if (sillas_libres > 0) {
            sillas_libres--;
            cout << "Cliente " << i << " se sienta y espera su turno." << endl;
            cliente_listo++;
            cv_barbero.notify_one();
        } else {
            cout << "Cliente " << i << " se va porque no hay sillas disponibles." << endl;
        }
    }

    void terminarServicio() {
        unique_lock<mutex> lock(mtx);
        cliente_sentado--;
        sillas_libres++;
    }

private:
    int sillas_libres;
    int cliente_listo;
    int cliente_sentado;
    mutex mtx;
    condition_variable cv_barbero;
};

Barberia barberia;

void cliente(int i) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1000, 2000);

    this_thread::sleep_for(chrono::milliseconds(dis(gen)));

    barberia.esperarTurno(i);
    barberia.cortarPelo(i);
    barberia.terminarServicio();
}

void barbero() {
    while (true) {
        barberia.cortarPelo(0);
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {
    thread t_barbero(barbero);

    thread t_clientes[N];
    for (int i = 0; i < N; ++i) {
        t_clientes[i] = thread(cliente, i);
    }

    for (int i = 0; i < N; ++i) {
        t_clientes[i].join();
    }

    t_barbero.join();

    return 0;
}