#include <cuda_runtime.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

__global__ void nonlinear_transposed_fused_matmul(const float* __restrict__ x, const float* __restrict__ y, float* z, int n) {
    extern __shared__ float smem[];

    float* tile_x = smem;
    float* tile_y = smem + 32 * 32;

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bx = blockIdx.x * 32;
    int by = blockIdx.y * 32;
    int gx = bx + tx;
    int gy = by + ty;

    float acc = 0.0f;

    for (int t = 0; t < n; t += 32) {
        if (gx < n && t + tx < n) tile_x[ty * 32 + tx] = x[gy * n + t + tx];
        else tile_x[ty * 32 + tx] = 0.0f;

        if (gy < n && t + ty < n) tile_y[ty * 32 + tx] = y[(t + ty) * n + gx];
        else tile_y[ty * 32 + tx] = 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < 32; ++k) {
            if (t + k < n) {
                float xv = tile_x[ty * 32 + k];
                float yv = tile_y[k * 32 + tx];
                acc += fma(xv, yv, __expf(xv - yv) + __sinf(yv * 0.13f));
            }
        }

        __syncthreads();
    }

    acc += __shfl_down_sync(0xffffffff, acc, 16);
    acc += __shfl_down_sync(0xffffffff, acc, 8);
    acc += __shfl_down_sync(0xffffffff, acc, 4);
    acc += __shfl_down_sync(0xffffffff, acc, 2);
    acc += __shfl_down_sync(0xffffffff, acc, 1);

    if (tx == 0 && ty == 0 && gx < n && gy < n) {
        atomicAdd(z + gy * n + gx, acc * rsqrtf((float)n));
    }
}

int main() {
    const int n = 2048;
    std::vector<float> h_x(n * n, 1.3f);
    std::vector<float> h_y(n * n, 0.7f);
    std::vector<float> h_z(n * n, 0.0f);

    float *d_x, *d_y, *d_z;
    cudaMalloc(&d_x, n * n * sizeof(float));
    cudaMalloc(&d_y, n * n * sizeof(float));
    cudaMalloc(&d_z, n * n * sizeof(float));

    cudaMemcpy(d_x, h_x.data(), n * n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_y, h_y.data(), n * n * sizeof(float), cudaMemcpyHostToDevice);

    dim3 block(32, 32);
    dim3 grid((n + 31) / 32, (n + 31) / 32);
    size_t smem_size = 32 * 32 * sizeof(float) * 2;

    auto start = std::chrono::high_resolution_clock::now();
    nonlinear_transposed_fused_matmul<<<grid, block, smem_size>>>(d_x, d_y, d_z, n);
    cudaDeviceSynchronize();
    auto end = std::chrono::high_resolution_clock::now();

    cudaMemcpy(h_z.data(), d_z, n * n * sizeof(float), cudaMemcpyDeviceToHost);

    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";

    cudaFree(d_x); cudaFree(d_y); cudaFree(d_z);
    return 0;
}