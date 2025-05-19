#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

#define USE_EMPLACE_BACK
// #define USE_PUSH_BACK

// Un **mutex** (exclusión mutua) se utiliza para asegurar que solo un hilo a la vez pueda 
// acceder a un recurso compartido, como una variable o una estructura de datos.
// Esto es importante porque, sin protección, múltiples hilos podrían intentar modificar el mismo 
// recurso al mismo tiempo

std::mutex mtx;

// Estructura de prueba.
// Vamos a modificar el campos
// dado un paso delta
struct Data{
    Data(int v) : value(v) {}
    int value;

    void mod (int delta){
        value += delta;
    }
};

// Delta como incremento
void hilo_prueba(std::shared_ptr<Data> shared_data, int delta){
    // Aquí utilizamos **std::lock_guard**, que simplifica el manejo de mutex. Al crear un 
    // objeto de tipo lock_guard y pasarlo un mutex, este se bloquea automáticamente. 
    // Esto significa que el mutex se mantendrá bloqueado mientras el lock_guard esté en 
    // el ámbito. Cuando el lock_guard sale del ámbito, ya sea porque la función ha terminado 
    // o se ha producido una excepción, el mutex se libera automáticamente. Esto ayuda a 
    // evitar olvidos y posibles bloqueos del programa, haciendo que nuestro código sea 
    // más seguro y fácil de mantener.
    std::lock_guard<std::mutex> lock(mtx);
    // Empieza el hilo
    std::cout << "---------------------------------------------------\n";
    std::cout << "[Hilo " << std::this_thread::get_id() << "] Iniciando con delta = " << delta << std::endl;

    // Modificamos el valor
    shared_data->mod(delta);
    std::cout << "[Hilo " << std::this_thread::get_id() << "] Modifico el valor a " << shared_data->value << std::endl;

    // Fin del hilo
    std::cout << "[Hilo " << std::this_thread::get_id() << "] Terminado." << std::endl;
    std::cout << "---------------------------------------------------\n";
}


int main(){
    auto t_inicial = std::chrono::steady_clock::now();

    // Para no usar raw pointers *
    // preferimos usar un puntero de tipo compartido
    // intente en su defecto usar un puntero único, verá el error
    // std::unique_ptr<Data> p1 = std::make_unique<Data>(10);
    std::shared_ptr<Data> p1 = std::make_shared<Data>(10);
    std::vector<std::thread> hilos;

    #ifdef USE_EMPLACE_BACK
        std::cout << "Usando emplace_back para agregar hilos." << std::endl;
    #else
        std::cout << "Usando push_back para agregar hilos." << std::endl;
    #endif

    // emplace_back es más versatil que push_back, en pocas palabras
    // push_back crea un objeto fuera del vector mientras que emplace_back 
    // crea el objeto dentro de este
    for(int i = 0; i < 5; i++){
        // Digamos, es como un constructor del tipo de objeto que se usará
        // "hilos" es un tipo de objeto <thread>, este recibe en su constructor
        // una función (en este caso, la prueba) y sus argumentos consecuentes
        // a tener n cantidad de argumentos en una función, la cantidad de argumentos para 
        // el constructor de un objeto tipo thread será n+1
        #ifdef USE_EMPLACE_BACK
            hilos.emplace_back(hilo_prueba, p1, i + 1);
        #else
            hilos.push_back(std::thread(hilo_prueba, p1, i + 1));
        #endif
    }

    for(auto &hilo: hilos){
        hilo.join();
    }
    
    auto t_final = std::chrono::steady_clock::now();
    auto t_total = std::chrono::duration_cast<std::chrono::microseconds>(t_final - t_inicial).count();

    std::cout << "Valor final: " << p1 -> value << std::endl;
    std::cout << "Tiempo total de ejecucion " << t_total << "ms" << std::endl;
}