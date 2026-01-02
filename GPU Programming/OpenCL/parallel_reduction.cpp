#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <vector>
#include <CL/cl.hpp>
#include <cmath>

int main() {
    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.empty()) {
            std::cerr << "No OpenCL platforms found." << std::endl;
            return 1;
        }

        cl::Platform platform = platforms[0];
        std::cout << "Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if (devices.empty()) {
            platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
            std::cout << "No GPU found, using CPU." << std::endl;
        }
        cl::Device device = devices[0];
        std::cout << "Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

        cl::Context context(device);
        cl::CommandQueue queue(context, device);

        // --------------------- Kernel Source ---------------------
        // Two kernels:
        // 1. mul: element-wise multiplication C[i] = A[i] * B[i]
        // 2. reduce: parallel sum reduction using local memory
        const char* kernelSource = R"(
            __kernel void mul(__global const float* A,
                              __global const float* B,
                              __global float* C,
                              const int n) {
                int i = get_global_id(0);
                if (i < n) {
                    C[i] = A[i] * B[i];
                }
            }

            __kernel void reduce(__global const float* input,
                                 __global float* output,
                                 __local float* shared,
                                 const int n) {
                int gid = get_global_id(0);
                int lid = get_local_id(0);
                int local_size = get_local_size(0);

                // Load data into local memory
                float value = (gid < n) ? input[gid] : 0.0f;
                shared[lid] = value;
                barrier(CLK_LOCAL_MEM_FENCE);

                // Parallel reduction in local memory
                for (int offset = local_size / 2; offset > 0; offset >>= 1) {
                    if (lid < offset) {
                        shared[lid] += shared[lid + offset];
                    }
                    barrier(CLK_LOCAL_MEM_FENCE);
                }

                // First thread writes partial sum
                if (lid == 0) {
                    output[get_group_id(0)] = shared[0];
                }
            }
        )";

        cl::Program program(context, kernelSource);
        program.build({device});

        cl::Kernel kernel_mul(program, "mul");
        cl::Kernel kernel_reduce(program, "reduce");

        const int N = 1 << 20;  // 1,048,576
        const int LOCAL_SIZE = 256;

        std::vector<float> A(N);
        std::vector<float> B(N);
        std::vector<float> C(N);           // intermediate: A[i]*B[i]
        std::vector<float> partial_sums((N + LOCAL_SIZE - 1) / LOCAL_SIZE);

        for (int i = 0; i < N; ++i) {
            A[i] = sinf(i) * 0.01f;
            B[i] = cosf(i) * 0.01f;
        }

        float cpu_dot = 0.0f;
        for (int i = 0; i < N; ++i) {
            cpu_dot += A[i] * B[i];
        }

        cl::Buffer bufA(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, A.data());
        cl::Buffer bufB(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, B.data());
        cl::Buffer bufC(context, CL_MEM_READ_WRITE, sizeof(float) * N, nullptr);
        cl::Buffer bufPartial(context, CL_MEM_WRITE_ONLY, sizeof(float) * partial_sums.size(), nullptr);

        kernel_mul.setArg(0, bufA);
        kernel_mul.setArg(1, bufB);
        kernel_mul.setArg(2, bufC);
        kernel_mul.setArg(3, N);

        queue.enqueueNDRangeKernel(kernel_mul, cl::NullRange, cl::NDRange(N), cl::NullRange);

        kernel_reduce.setArg(0, bufC);
        kernel_reduce.setArg(1, bufPartial);
        kernel_reduce.setArg(2, cl::Local(LOCAL_SIZE * sizeof(float)));
        kernel_reduce.setArg(3, N);

        int global_size = partial_sums.size() * LOCAL_SIZE;
        queue.enqueueNDRangeKernel(kernel_reduce, cl::NullRange,
                                   cl::NDRange(global_size), cl::NDRange(LOCAL_SIZE));
        queue.enqueueReadBuffer(bufPartial, CL_TRUE, 0,
                                sizeof(float) * partial_sums.size(), partial_sums.data());

        float gpu_dot = 0.0f;
        for (float s : partial_sums) {
            gpu_dot += s;
        }

        std::cout << "CPU dot product : " << cpu_dot << std::endl;
        std::cout << "GPU dot product : " << gpu_dot << std::endl;
        std::cout << "Absolute error  : " << std::abs(cpu_dot - gpu_dot) << std::endl;
        std::cout << "Verification    : " << (std::abs(cpu_dot - gpu_dot) < 1e-4f ? "PASSED" : "FAILED") << std::endl;

    } catch (cl::Error& e) {
        std::cerr << "OpenCL error: " << e.what() << " (" << e.err() << ")" << std::endl;
        return 1;
    }

    return 0;
}