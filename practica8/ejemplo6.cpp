#include <iostream>
#include <thread>
#include <semaphore.h>
using namespace std;

sem_t libres, mutex, ocupados;
const int n=3;
int fin, frente, elemento, cola[n];

void Productor(){
	while(true){
		elemento=rand() % 10+1;
		sem_wait(&libres);
		sem_wait(&mutex);
		cola[fin]=elemento;
		fin=(fin+1)%n;
		cout << "ELEMENTO PRODUCIDO: " << elemento << endl;
		sem_post(&mutex);
		sem_post(&ocupados);
	}
}

void Consumidor(){
	while(true){
		sem_wait(&ocupados);
		sem_wait(&mutex);
		elemento=cola[frente];
		frente=(frente + 1)%n;
		sem_post(&mutex);
		sem_post(&libres);
		cout << "ELEMENTO CONSUMIDO: " << elemento << endl;
	}
}
int main(){
	
	fin=n;
	frente=0;
	
	sem_init(&mutex, 0, 1);
	sem_init(&ocupados, 0, 0);
	sem_init(&libres, 0, n);
	
	thread productor(Productor);
	thread consumidor(Consumidor);
	
	productor.join();
	consumidor.join();
	return 0;
}