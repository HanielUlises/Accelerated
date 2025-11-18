#include <omp.h>
#include <stdio.h>

// Recursive function that calculates the n-th Fibonacci number
// It uses recursion to sum the two previous numbers
// until n is less than 2
long long fib(int n) {
    return (n < 2 ? 1 : fib(n-1) + fib(n-2));
}

int main() {
    // Define the initial value for the Fibonacci calculation
    int n = 45;

    // Start a parallel region with OpenMP, where each thread
    // will execute the code within this block
    #pragma omp parallel
    {
        // Get the current thread number
        int t = omp_get_thread_num();
        
        // Print the result of the Fibonacci calculation
        // The Fibonacci number calculated varies depending on the thread
        // It is passed as an argument to fib(n + t), where t is the thread number
        printf("%d:  %lld\n", t, fib(n + t));
    }

    return 0;
}