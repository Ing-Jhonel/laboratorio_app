#include <iostream> // Para entrada y salida estándar
#include <thread> // Para manejo de hilos
#include <mutex> // Para exclusión mutua
#include <condition_variable> // Para sincronización entre hilos
#include <queue> // Para las colas de mensajes

using namespace std;

mutex mtx; // Mutex para proteger secciones críticas
condition_variable cv_emisor, cv_traductor, cv_receptor; // Variables de condición para sincronizar hilos

queue<int> cola_emisor_traductor; // Cola del emisor al traductor
queue<int> cola_traductor_receptor; // Cola del traductor al receptor

bool emisor_listo = false; // Señal de que el emisor ha enviado un mensaje
bool traductor_listo = false; // Señal de que el traductor ha traducido un mensaje
bool receptor_listo = false; // Señal de que el receptor ha recibido un mensaje

const int N = 5; // Cantidad de mensajes a enviar

void emisor() {
    for (int i = 1; i <= N; ++i) { // Bucle para enviar N mensajes
        unique_lock<mutex> lock(mtx); // Adquiere el lock del mutex
        cola_emisor_traductor.push(i); // Inserta mensaje en la cola emisor → traductor
        cout << "Emisor emitio: " << i << endl; // Imprime mensaje enviado
        emisor_listo = true; // Indica que hay un mensaje listo
        cv_traductor.notify_one(); // Notifica al traductor que puede trabajar

        // Espera confirmación del receptor
        cv_emisor.wait(lock, [] { return receptor_listo; }); // Espera hasta que receptor_listo sea true
        receptor_listo = false; // Resetea la señal para la siguiente iteración
        cout << "Emisor es notificado con recepcion, puede continuar o finalizar." << endl; // Mensaje de confirmación
    }
}

void traductor() {
    for (int i = 1; i <= N; ++i) { // Bucle para procesar N mensajes
        unique_lock<mutex> lock(mtx); // Adquiere el lock del mutex
        cv_traductor.wait(lock, [] { return emisor_listo && !cola_emisor_traductor.empty(); }); // Espera mensaje del emisor

        int valor = cola_emisor_traductor.front(); // Toma el mensaje de la cola
        cola_emisor_traductor.pop(); // Elimina el mensaje de la cola
        int traducido = valor * 10; // Aplica "traducción" (multiplica por 10)
        cola_traductor_receptor.push(traducido); // Encola mensaje traducido
        cout << "Traductor proceso: " << valor << " a " << traducido << endl; // Imprime mensaje traducido
        emisor_listo = false; // Resetea señal del emisor
        traductor_listo = true; // Señala que hay un mensaje traducido listo
        cv_receptor.notify_one(); // Notifica al receptor que puede continuar
    }
}

void receptor() {
    for (int i = 1; i <= N; ++i) { // Bucle para recibir N mensajes traducidos
        unique_lock<mutex> lock(mtx); // Adquiere el lock del mutex
        cv_receptor.wait(lock, [] { return traductor_listo && !cola_traductor_receptor.empty(); }); // Espera mensaje traducido

        int valor = cola_traductor_receptor.front(); // Toma el mensaje traducido
        cola_traductor_receptor.pop(); // Elimina el mensaje de la cola
        cout << "Receptor recibio: " << valor << endl; // Imprime el mensaje recibido
        traductor_listo = false; // Resetea señal del traductor
        receptor_listo = true; // Señala que receptor ha terminado
        cv_emisor.notify_one(); // Notifica al emisor que puede continuar
    }
}

int main() {
    thread t1(emisor); // Crea el hilo del emisor
    thread t2(traductor); // Crea el hilo del traductor
    thread t3(receptor); // Crea el hilo del receptor

    t1.join(); // Espera a que termine el hilo del emisor
    t2.join(); // Espera a que termine el hilo del traductor
    t3.join(); // Espera a que termine el hilo del receptor

    return 0; // Fin del programa
}
