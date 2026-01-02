#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <vector>
#include <cmath>
#include <CL/cl.hpp>

// y[i] = sin(x[i]) + log(x[i] + 1)

int main() {
    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.empty()) return 1;

        cl::Platform platform = platforms[0];

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if (devices.empty()) {
            platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
        }
        if (devices.empty()) return 1;

        cl::Device device = devices[0];
        cl::Context context(device);
        cl::CommandQueue queue(context, device);

        const char* kernelSource = R"(
            __kernel void nonlinear(const int n,
                                     __global const float* x,
                                     __global float* y) {
                int i = get_global_id(0);
                if (i < n) {
                    y[i] = sin(x[i]) + log(x[i] + 1.0f);
                }
            }
        )";

        cl::Program program(context, kernelSource);

        try {
            program.build({device});
        } catch (...) {
            std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
            return 1;
        }

        cl::Kernel kernel(program, "nonlinear");

        const int N = 1 << 24;

        std::vector<float> X(N);
        std::vector<float> Y(N, 0.0f);

        for (int i = 0; i < N; ++i) {
            X[i] = 0.001f * i;
        }

        cl::Buffer bufX(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, X.data());
        cl::Buffer bufY(context, CL_MEM_WRITE_ONLY, sizeof(float) * N);

        kernel.setArg(0, N);
        kernel.setArg(1, bufX);
        kernel.setArg(2, bufY);

        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(N), cl::NullRange);
        queue.finish();
        queue.enqueueReadBuffer(bufY, CL_TRUE, 0, sizeof(float) * N, Y.data());

        bool ok = true;
        const float eps = 1e-5f;

        for (int i = 0; i < 10; ++i) {
            float expected = std::sin(X[i]) + std::log(X[i] + 1.0f);
            if (std::fabs(Y[i] - expected) > eps) {
                ok = false;
                break;
            }
        }

        std::cout << "Result: " << (ok ? "correct" : "incorrect") << std::endl;

    } catch (cl::Error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
