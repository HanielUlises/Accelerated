#include <cuda_runtime.h>

__global__ void double_kernel(float* data, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) data[i] *= 2.0f;
}

extern "C" void solve(float* data, int n) {
    int threads = 256;
    int blocks = (n + threads - 1) / threads;

    double_kernel<<<blocks, threads>>>(data, n);
    cudaDeviceSynchronize();
}
