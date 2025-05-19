#include <omp.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Declaramos la variable que almacenará el valor máximo del ciclo
    int max;

    // Convertimos el argumento pasado por la línea de comandos a entero
    // El valor se obtiene de argv[1], que es el primer argumento del programa
    sscanf(argv[1], "%d", &max);

    // Iniciamos una región paralela con OpenMP utilizando un ciclo for paralelo
    // Cada iteración del ciclo se asigna a diferentes hilos
    #pragma omp parallel for
    for (int i = 1; i <= max; i++) {
        // Imprimimos el número del hilo que está ejecutando la iteración actual
        // y el valor de la iteración en curso
        printf("%d: %d\n", omp_get_thread_num(), i);
    }

    return 0;
}
