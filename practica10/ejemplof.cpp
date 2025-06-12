 #include <iostream>
#include <mutex>
#include <thread>
using namespace std;

int main(){
const int no_of_philosophers = 5;
 	// estructura de palillos
	struct Chopstics{
		public:
		Chopstics(){;}
		mutex mu;
	};
	auto eat = [](Chopstics &left_chopstics, Chopstics& right_chopstics, int philosopher_number) {
		unique_lock<std::mutex> llock(left_chopstics.mu);
		unique_lock<std::mutex> rlock(right_chopstics.mu);
		cout << "Filosofo " << philosopher_number << " esta comiendo" << endl;
		chrono::milliseconds timeout(1500);
		this_thread::sleep_for(timeout);
		cout << "Filosofo " << philosopher_number << " ha terminado de comer" << endl;
	};
	 //crear palillos
	Chopstics chp[no_of_philosophers];
	 //create Filosofos
	thread philosopher[no_of_philosophers];
	 //Filosofo empieza a leer
	cout << "Filosofo " << (0+1) << " esta leyendo.." << endl;
	philosopher[0] = thread(eat, ref(chp[0]), ref(chp[no_of_philosophers]), (0+1));
	for(int i = 1; i < no_of_philosophers; ++i) {
		cout << "Filosofo " << (i+1) << " esta leyendo.." << endl;
		philosopher[i] = thread(eat, ref(chp[i]), ref(chp[i-1]), (i+1));
	}
	for(auto &ph: philosopher) {
		ph.join();
	}
return 0;
}