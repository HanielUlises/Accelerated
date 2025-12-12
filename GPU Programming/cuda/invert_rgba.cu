#include <cuda_runtime.h>

__global__ void invert_kernel(unsigned char* image, int width, int height) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int totalPixels = width * height;

    if (idx >= totalPixels) return;

    int base = idx * 4;

    image[base + 0] = 255 - image[base + 0]; // R
    image[base + 1] = 255 - image[base + 1]; // G
    image[base + 2] = 255 - image[base + 2]; // B
    // Alpha remains unchanged
    // image[base + 3] stays the same
}

extern "C" void solve(unsigned char* image, int width, int height) {
    int threadsPerBlock = 256;
    int blocksPerGrid = (width * height + threadsPerBlock - 1) / threadsPerBlock;

    invert_kernel<<<blocksPerGrid, threadsPerBlock>>>(image, width, height);
    cudaDeviceSynchronize();
}
