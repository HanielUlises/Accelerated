#include <omp.h>
#include <stdio.h>

// Función recursiva que calcula el n-ésimo número de Fibonacci
// Se utiliza recursión para sumar los dos números anteriores
// al número dado, hasta que n sea menor a 2
long long fib(int n) {
    return (n < 2 ? 1 : fib(n-1) + fib(n-2));
}

int main() {
    // Definimos el valor inicial para el cálculo de Fibonacci
    int n = 45;

    // Iniciamos una región paralela con OpenMP, donde cada hilo
    // ejecutará el código dentro de este bloque
    #pragma omp parallel
    {
        // Obtenemos el número del hilo actual
        int t = omp_get_thread_num();
        
        // Imprimimos el resultado del cálculo de Fibonacci
        // El número de Fibonacci que se calcula varía según el hilo
        // Se pasa como argumento a fib(n + t), donde t es el número de hilo
        printf("%d:  %lld\n", t, fib(n + t));
    }

    return 0;
}
