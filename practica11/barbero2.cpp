#include <iostream>
#include <semaphore.h>
#include <thread>
#include <chrono>
using namespace std;

const int NUM_SILLAS = 5;
int enEspera = 0;

sem_t mutex;
sem_t sem_clientes;
sem_t sem_barbero;

void Barbero() {
	while (true) {
		sem_wait(&sem_clientes);
		sem_wait(&mutex);
		enEspera--;
		cout << "Barbero: Cortando el cabello..." << endl;
		sem_post(&mutex);
		
		//this_thread::sleep_for(chrono::seconds(2));

		sem_post(&sem_barbero);
	}
}

void Clientela() {
	while (true) {
		sem_wait(&mutex);
		if (enEspera < NUM_SILLAS) {
			enEspera++;
			cout << "Cliente: Esperando (en cola: " << enEspera << ")" << endl;
			sem_post(&sem_clientes);
			sem_post(&mutex);
			
			sem_wait(&sem_barbero);   // Espera corte
			cout << "Cliente: Me cortaron el cabello." << endl;
		} else {
			cout << "Cliente: No hay sillas, me voy." << endl;
			sem_post(&mutex);
		}
		//this_thread::sleep_for(chrono::milliseconds(500));  // Nuevos clientes cada medio segundo
	}
}

int main() {
	sem_init(&mutex, 0, 1);
	sem_init(&sem_clientes, 0, 0);
	sem_init(&sem_barbero, 0, 0);

	thread barbero(Barbero);
	thread cliente1(Clientela);
	thread cliente2(Clientela);
	thread cliente3(Clientela);
	thread cliente4(Clientela);
	thread cliente5(Clientela);

	barbero.join();
	cliente1.join();
	cliente2.join();
	cliente3.join();
	cliente4.join();
	cliente5.join();

	return 0;
}
