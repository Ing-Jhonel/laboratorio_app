#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
using namespace std;
#define n 5

/*struct Palillos{
	mutex mut;
};*/
sem_t palillo[n];
sem_t sitios;
void filosofos(int i){
	while(true){
		cout << "Filosofo " << i << " : Piensa" << endl;
		this_thread::sleep_for(chrono::milliseconds(500));
		sem_wait(&sitios);
		sem_wait(&palillo[i]);
		sem_wait(&palillo[i+1 % n]);
		cout << "Filosofo " << i << " : Come" << endl;
		this_thread::sleep_for(chrono::milliseconds(500));
		sem_post(&palillo[i]);
		sem_post(&palillo[(i+1) % n]);
		sem_post(&sitios);
	}
}

int main(){
	sem_init(&sitios, 0, 4);
	for(int i=0; i<n; i++){
		sem_init(&palillo[i], 0, 1);
	}
	
	thread f[n];
	for (int i=0; i<n; i++) {
        f[i] = thread(filosofos, i);
    }
    
    for(int i=0; i<n; i++){
    	f[i].join();
	}
	return 0;
}