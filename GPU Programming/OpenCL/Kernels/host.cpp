#include <CL/cl.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>

std::string load_source(const std::string& path) {
    std::ifstream f(path);
    return std::string((std::istreambuf_iterator<char>(f)), {});
}

int main() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms[0];

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];

    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    std::string source = load_source("nonlinear_tiled_matmul.cl");
    cl::Program program(context, source);
    program.build({device});

    cl::Kernel kernel(program, "nonlinear_transposed_fused_matmul");

    const int n = 2048;
    std::vector<float> input_x(n * n, 1.3f);
    std::vector<float> input_y(n * n, 0.7f);
    std::vector<float> output_z(n * n, 0.0f);

    cl::Buffer buf_x(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n * n, input_x.data());
    cl::Buffer buf_y(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n * n, input_y.data());
    cl::Buffer buf_z(context, CL_MEM_WRITE_ONLY, sizeof(float) * n * n);

    kernel.setArg(0, buf_x);
    kernel.setArg(1, buf_y);
    kernel.setArg(2, buf_z);
    kernel.setArg(3, n);

    cl::NDRange global(n, n);
    cl::NDRange local(32, 8);

    auto start = std::chrono::high_resolution_clock::now();
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
    queue.enqueueReadBuffer(buf_z, CL_TRUE, 0, sizeof(float) * n * n, output_z.data());
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";

    return 0;
}