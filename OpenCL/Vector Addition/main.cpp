#include <CL/opencl.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

std::string load_kernel_source(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open kernel file " << filename << std::endl;
        exit(1);
    }
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

int main() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms.front(); 

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices.front(); 

    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    std::string kernelSource = load_kernel_source("vector_add.cl");
    cl::Program::Sources sources(1, {kernelSource.c_str(), kernelSource.length()});
    cl::Program program(context, sources);
    program.build({device});

    cl::Kernel kernel(program, "vector_add");

    const int numElements = 1024;
    std::vector<int> A(numElements);
    std::vector<int> B(numElements);
    std::vector<int> C(numElements); // Result vector

    for (int i = 0; i < numElements; ++i) {
        A[i] = i;
        B[i] = i * 2;
    }

    cl::Buffer bufferA(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * numElements, A.data());
    cl::Buffer bufferB(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * numElements, B.data());
    cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, sizeof(int) * numElements);

    kernel.setArg(0, bufferC);
    kernel.setArg(1, bufferA);
    kernel.setArg(2, bufferB);
    kernel.setArg(3, numElements);

    cl::NDRange globalSize(numElements); 
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);
    queue.finish(); 

    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, sizeof(int) * numElements, C.data());

    for (int i = 0; i < 10; ++i) {
        std::cout << "C[" << i << "] = " << C[i] << std::endl;
    }

    return 0;
}