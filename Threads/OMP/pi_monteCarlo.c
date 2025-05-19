#include <stdio.h>
#include <omp.h>

typedef unsigned int ui;
// Random number between 0 and 1 using a linear congruential generator
// This function updates the seed with each call
double rnd(ui *seed) {
    *seed = (1140671485 * (*seed) + 12820163) % (1 << 24);
    return ((double) (*seed)) / (1 << 24);
}

int main(int argc, char *argv[]) {
    int num_shots;
    sscanf(argv[1], "%i", &num_shots);
    int num_hits = 0;
    #pragma omp parallel reduction(+:num_hits)
    {
        ui seed = omp_get_thread_num();
        // Calculate the number of "shots" each thread should perform
        int local_shots = (num_shots / omp_get_num_threads()) +
                          ((num_shots % omp_get_num_threads() > omp_get_thread_num()) ? 1 : 0);
        // Generate points and check how many fall inside the circle
        while (local_shots-- > 0) {
            double x = rnd(&seed);
            double y = rnd(&seed);
            // Hit
            if (x * x + y * y <= 1) num_hits = num_hits + 1;
        }
    }
    double pi = 4.0 * (double)num_hits / (double)num_shots;
    printf("%lf\n", pi);
    return 0;
}