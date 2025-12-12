#include <cuda_runtime.h>

__global__ void zero_kernel(int* data, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) data[i] = 0;
}

extern "C" void solve(int* data, int n) {
    int threads = 256;
    int blocks = (n + threads - 1) / threads;

    zero_kernel<<<blocks, threads>>>(data, n);
    cudaDeviceSynchronize();
}
