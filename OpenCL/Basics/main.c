#include <stdio.h>
#include <stdlib.h>

#include <CL/opencl.h>

// OpenCL kernel
const char *kernel_source = 
            "__kernel void hello_kernel(__global char *output){"
            "   const char message [] = \"Hello, World!\\n\";"
            "   int tid = get_global_id(0);"
            "   output[tid] = message[tid];"
            "}";

// often doesn't require input or output data to be transferred 
// between the host and the device. The kernel function itself can 
// perform the printing operation directly.
int main() {
    // Get platform and device
    cl_platform_id platform;
    cl_device_id device;
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    // OpenCL context and command queue
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    // Memory buffer
    char output[14] = {0};
    cl_mem output_buffer = clCreateBuffer(context, 1, &kernel_source, NULL, NULL);
    
    // Compiling kernel
    cl_program program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "hello_kernel", NULL);

    // Kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &output_buffer);

    // Enqueuing kernel execution
    size_t global_size = sizeof(output);
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

    // All enqueued tasks are finished
    clFinish(queue);

    clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, sizeof(output), output, 0, NULL, NULL);
    printf("%s", output);

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(output_buffer);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}