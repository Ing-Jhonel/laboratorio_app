#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>

using namespace std;

const int NUM_FILOSOFOS = 5;

mutex mtx_palillos[NUM_FILOSOFOS];
mutex mtx_mensajes;
condition_variable cv;
queue<string> mensajes;
bool mensaje_disponible = false;

void enviar_mensaje(const string& msg) {
    unique_lock<mutex> lock(mtx_mensajes);
    mensajes.push(msg);
    mensaje_disponible = true;
    cv.notify_one();
}

void recibir_mensaje() {
    unique_lock<mutex> lock(mtx_mensajes);
    cv.wait(lock, []{ return mensaje_disponible; });

    string msg = mensajes.front();
    mensajes.pop();
    cout << msg << endl;

    if(mensajes.empty()) {
        mensaje_disponible = false;
    }
    lock.unlock();
}

void filosofo_par(int id) {
    while(true) {
        enviar_mensaje("Filosofo " + to_string(id) + " esta pensando");

        unique_lock<mutex> palillo_der(mtx_palillos[(id + 1) % NUM_FILOSOFOS], defer_lock);
        unique_lock<mutex> palillo_izq(mtx_palillos[id], defer_lock);

        lock(palillo_der, palillo_izq);

        enviar_mensaje("Filosofo " + to_string(id) + " esta comiendo");

        this_thread::sleep_for(chrono::seconds(1));
    }
}

void filosofo_impar(int id) {
    while(true) {
        enviar_mensaje("Filosofo " + to_string(id) + " esta pensando");

        unique_lock<mutex> palillo_izq(mtx_palillos[id], defer_lock);
        unique_lock<mutex> palillo_der(mtx_palillos[(id + 1) % NUM_FILOSOFOS], defer_lock);

        lock(palillo_izq, palillo_der);

        enviar_mensaje("Filosofo " + to_string(id) + " esta comiendo");

        this_thread::sleep_for(chrono::seconds(1));
    }
}

void receptor_mensajes() {
    while(true) {
        recibir_mensaje();
    }
}

int main() {
    thread receptor(receptor_mensajes);

    thread filosofos[NUM_FILOSOFOS];
    for(int i = 0; i < NUM_FILOSOFOS; ++i) {
        if(i == 0 || i == 1) {
            filosofos[i] = thread(filosofo_par, i);
        } else {
            filosofos[i] = thread(filosofo_impar, i);
        }
    }

    receptor.join();
    for(int i = 0; i < NUM_FILOSOFOS; ++i) {
        filosofos[i].join();
    }

    return 0;
}
