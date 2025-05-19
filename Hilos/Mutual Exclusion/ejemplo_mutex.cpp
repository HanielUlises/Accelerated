#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <random>

class CuentaBancaria {
private:
    int saldo;
    std::mutex mtx;  // Mutex, para proteger el saldo

public:
    CuentaBancaria(int saldo_inicial) : saldo(saldo_inicial) {}

    // Método para transferir dinero entre cuentas
    bool transferir(CuentaBancaria &destino, int cantidad) {
        // Bloqueo en ambos mutex para prevenir condiciones de carrera
        std::unique_lock<std::mutex> lock_origen(mtx, std::defer_lock);
        std::unique_lock<std::mutex> lock_destino(destino.mtx, std::defer_lock);

        // Evita interbloqueos
        std::lock(lock_origen, lock_destino); 

        if (saldo >= cantidad) {
            saldo -= cantidad;
            destino.saldo += cantidad;
            std::cout << "Transferencia de " << cantidad << " realizada.\n";
            return true;
        } else {
            std::cout << "Saldo insuficiente para transferir " << cantidad << ".\n";
            return false;
        }
    }

    int obtenerSaldo() const {
        return saldo;
    }
};

void Transfe(CuentaBancaria &cuenta1, CuentaBancaria &cuenta2, int num_transferencias) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);

    for (int i = 0; i < num_transferencias; ++i) {
        int monto = dist(gen);
        if (!cuenta1.transferir(cuenta2, monto)) {
            // Si no se puede transferir desde cuenta1 a cuenta2, intentamos al revés
            cuenta2.transferir(cuenta1, monto);
        } 
        // Pausa para (simulacion) tiempo de procesamiento
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    CuentaBancaria cuenta1(1000);
    CuentaBancaria cuenta2(1000);

    // Iniciar varios hilos para transferencias concurrentes
    std::thread t1(Transfe, std::ref(cuenta1), std::ref(cuenta2), 10);
    std::thread t2(Transfe, std::ref(cuenta2), std::ref(cuenta1), 10);
    std::thread t3(Transfe, std::ref(cuenta1), std::ref(cuenta2), 10);
    std::thread t4(Transfe, std::ref(cuenta2), std::ref(cuenta1), 10);

    // Esperar a que todos los hilos terminen
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "Saldo final cuenta 1: " << cuenta1.obtenerSaldo() << "\n";
    std::cout << "Saldo final cuenta 2: " << cuenta2.obtenerSaldo() << "\n";

    return 0;
}
