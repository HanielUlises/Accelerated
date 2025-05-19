#include <omp.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Declare the variable that will store the maximum loop value
    int max;

    // Convert the command-line argument to an integer
    // The value is obtained from argv[1], which is the first program argument
    sscanf(argv[1], "%d", &max);

    // Start a parallel region with OpenMP using a parallel for loop
    // Each iteration of the loop is assigned to different threads
    #pragma omp parallel for
    for (int i = 1; i <= max; i++) {
        // Print the thread number executing the current iteration
        // and the value of the current iteration
        printf("%d: %d\n", omp_get_thread_num(), i);
    }

    return 0;
}