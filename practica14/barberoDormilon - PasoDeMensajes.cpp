#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

const int NUM_SILLAS = 5;

mutex mtx;
condition_variable cv_barbero;
condition_variable cv_cliente;
queue<int> cola_clientes;
int cliente_id = 0;
bool barbero_ocupado = false;

void barbero() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv_barbero.wait(lock, [] { return !cola_clientes.empty(); });

        int id = cola_clientes.front();
        cola_clientes.pop();
        barbero_ocupado = true;

        cout << "Barbero esta cortando el cabello al cliente " << id << endl;

        lock.unlock();
        this_thread::sleep_for(chrono::seconds(2));

        lock.lock();
        barbero_ocupado = false;
        cout << "Barbero termino con el cliente " << id << endl;

        cv_cliente.notify_all();
    }
}

void cliente(int id) {
    unique_lock<mutex> lock(mtx);

    if (cola_clientes.size() < NUM_SILLAS) {
        cola_clientes.push(id);
        cout << "Cliente " << id << " espera en la sala." << endl;

        cv_barbero.notify_one();

        cv_cliente.wait(lock, [] { return !barbero_ocupado; });
        cout << "Cliente " << id << " fue atendido y se va." << endl;
    } else {
        cout << "Cliente " << id << " se va porque no hay sillas disponibles." << endl;
    }
}

int main() {
    thread t_barbero(barbero);

    vector<thread> clientes;

    for (int i = 0; i < 10; ++i) {
        this_thread::sleep_for(chrono::milliseconds(500));
        clientes.push_back(thread(cliente, cliente_id++));
    }

    for (auto& t : clientes) {
        t.join();
    }

    t_barbero.detach();
    return 0;
}
