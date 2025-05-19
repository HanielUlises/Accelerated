#include <stdio.h>
#include <omp.h>

typedef unsigned int ui;
// Número aleatorio entre 0 y 1 utilizando un generador congruencial
// Esta función actualiza la semilla en cada llamada
double rnd(ui *seed){
    *seed = (1140671485 * (*seed) + 12820163) % (1 << 24);
    return ((double) (*seed)) / (1 << 24);
}

int main (int argc, char *argv[]){
    int num_shots;
    sscanf(argv[1], "%i", &num_shots);
    int num_hits = 0;
    #pragma omp parallel reduction (+:num_hits)
    {
        ui seed = omp_get_thread_num();
        // Calculamos la cantidad de "disparos" que cada hilo debe hacer
        int loc_shots = (num_shots/omp_get_thread_num()) + 
        ((num_shots % omp_get_num_threads() > omp_get_thread_num() ? 1 : 0));
        // Generamos puntos y vemos cuantos están dentro del círculo
        while(loc_shots-- > 0){
            double x = rnd (&seed);
            double y = rnd (&seed);
            // Acertamos
            if(x * x + y * y <= 1) num_hits = num_hits + 1; 
        }
    }
    double pi = 4.0 * (double) num_hits/(double)num_shots;
    printf("lf\n", pi);
    return 0;
}