#include <iostream>
#include <thread>
#include <semaphore.h>

using namespace std;
sem_t libres, mutex, ocupados;
const int n=3;
int fin=0, frente=0, elemento1, elemento2, elemento, cola[n];

void Productor1(){
	while(true){
		elemento1=rand() % 10+1;
		sem_wait(&libres);
		sem_wait(&mutex);
		cola[fin]=elemento1;
		fin=(fin+1)%n;
		cout << "ELEMENTO 1 PRODUCIDO: " << elemento1 << endl;
		sem_post(&mutex);
		sem_post(&ocupados);
	}
}

void Productor2(){
	while(true){
		elemento2=rand() % 10+1;
		sem_wait(&libres);
		sem_wait(&mutex);
		cola[fin]=elemento2;
		fin=(fin+1)%n;
		cout << "ELEMENTO 2 PRODUCIDO: " << elemento2 << endl;
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
	
	thread productor1(Productor1);
	thread productor2(Productor2);
	thread consumidor(Consumidor);
	
	productor1.join();
	productor2.join();
	consumidor.join();
	return 0;
}