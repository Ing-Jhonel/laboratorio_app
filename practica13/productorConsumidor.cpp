#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<chrono>

using namespace std;

const int N=10;

class Buffer{
private:
    int cola[N];
    int frente=0;
    int final=0;
    int tam=0;

    mutex mtx;
    condition_variable clleno,cvacio;

public:
    void insertar(int elemento){
        unique_lock<mutex>lock(mtx);
        while(tam==N){
            clleno.wait(lock);
        }
        cola[final]=elemento;
        final=(final+1)%N;
        tam++;
        cout<<"Produciendo:"<<elemento<<endl;
        cvacio.notify_one();
    }

    void extraer(int&x){
        unique_lock<mutex>lock(mtx);
        while(tam==0){
            cvacio.wait(lock);
        }
        x=cola[frente];
        frente=(frente+1)%N;
        tam--;
        cout<<"Consumiendo:"<<x<<endl<<endl;
        clleno.notify_one();
    }
};

Buffer buffer;

void productor(){
    while(true){
        int y=rand()%10;
        buffer.insertar(y);
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void consumidor(){
    while(true){
        int a;
        buffer.extraer(a);
        this_thread::sleep_for(chrono::milliseconds(150));
    }
}

int main(){
    thread t1(productor);
    thread t2(consumidor);
    t1.join();
    t2.join();
    return 0;
}