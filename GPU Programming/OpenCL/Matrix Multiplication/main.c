// matrix_multiply_opencl.c
#define CL_TARGET_OPENCL_VERSION 120
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define CHECK_ERROR(err, msg) \
    if (err != CL_SUCCESS) { \
        fprintf(stderr, "%s (Error code: %d)\n", msg, err); \
        exit(EXIT_FAILURE); \
    }

// Matrix dimensions (can be changed)
#define M 1024  // rows of A and C
#define N 1536  // cols of B and C
#define K 1280  // cols of A, rows of B

// Local work size (tune for your GPU)
#define TS 16   // Tile size (16x16 is good for most GPUs)

const char *kernel_source = 
"__kernel void matmul(__global const float* A,\n"
"                     __global const float* B,\n"
"                     __global float* C,\n"
"                     const int M, const int N, const int K)\n"
"{\n"
"    const int row = get_local_id(0); // Local row index (0..TS-1)\n"
"    const int col = get_local_id(1); // Local col index (0..TS-1)\n"
"    const int globalRow = TS * get_group_id(0) + row;\n"
"    const int globalCol = TS * get_group_id(1) + col;\n"
"\n"
"    __local float Asub[TS][TS];\n"
"    __local float Bsub[TS][TS];\n"
"\n"
"    float acc = 0.0f;\n"
"    const int numTiles = (K + TS - 1) / TS;\n"
"\n"
"    for (int t = 0; t < numTiles; t++) {\n"
"        // Load tiles collaboratively\n"
"        int tiledRow = TS * t + row;\n"
"        int tiledCol = TS * t + col;\n"
"        Asub[row][col] = (globalRow < M && tiledCol < K) ? A[globalRow * K + tiledCol] : 0.0f;\n"
"        Bsub[row][col] = (tiledRow < K && globalCol < N) ? B[tiledRow * N + globalCol] : 0.0f;\n"
"\n"
"        barrier(CLK_LOCAL_MEM_FENCE);\n"
"\n"
"        // Accumulate tile\n"
"        for (int k = 0; k < TS; k++) {\n"
"            if (TS * t + k < K) // avoid over-read\n"
"                acc += Asub[row][k] * Bsub[k][col];\n"
"        }\n"
"        barrier(CLK_LOCAL_MEM_FENCE);\n"
"    }\n"
"\n"
"    if (globalRow < M && globalCol < N)\n"
"        C[globalRow * N + globalCol] = acc;\n"
"}\n";

int main() {
    cl_int err;
    cl_platform_id platform = NULL;
    cl_device_id device = NULL;
    cl_context context = NULL;
    cl_command_queue queue = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;

    float *A, *B, *C_host, *C_gpu;
    cl_mem d_A, d_B, d_C;

    // Allocate and initialize host matrices
    A = (float*)malloc(M * K * sizeof(float));
    B = (float*)malloc(K * N * sizeof(float));
    C_host = (float*)malloc(M * N * sizeof(float));
    C_gpu = (float*)malloc(M * N * sizeof(float));

    printf("Initializing %dx%d * %dx%d matrices...\n", M, K, K, N);
    for (int i = 0; i < M * K; i++) A[i] = (float)(rand() % 100) / 10.0f;
    for (int i = 0; i < K * N; i++) B[i] = (float)(rand() % 100) / 10.0f;
    memset(C_host, 0, M * N * sizeof(float));
    memset(C_gpu, 0, M * N * sizeof(float));

    // Get platform
    err = clGetPlatformIDs(1, &platform, NULL);
    CHECK_ERROR(err, "Failed to get platform");

    // Prefer GPU, fallback to CPU
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err == CL_DEVICE_NOT_FOUND) {
        printf("GPU not found, falling back to CPU\n");
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
    }
    CHECK_ERROR(err, "Failed to get device");

    // Print device info
    char name[128], vendor[128];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, NULL);
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(vendor), vendor, NULL);
    printf("Using device: %s (%s)\n", name, vendor);

    // Create context and queue
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_ERROR(err, "Failed to create context");

    #ifdef CL_VERSION_2_0
    queue = clCreateCommandQueueWithProperties(context, device, 0, &err);
    if (err == CL_INVALID_VALUE) // Fallback for older drivers
    #endif
    queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    CHECK_ERROR(err, "Failed to create command queue");

    // Create buffers
    d_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                         M * K * sizeof(float), A, &err);
    CHECK_ERROR(err, "Failed to create buffer A");

    d_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                         K * N * sizeof(float), B, &err);
    CHECK_ERROR(err, "Failed to create buffer B");

    d_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                         M * N * sizeof(float), NULL, &err);
    CHECK_ERROR(err, "Failed to create buffer C");

    // Build program
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    CHECK_ERROR(err, "Failed to create program");

    err = clBuildProgram(program, 1, &device, "-cl-fast-relaxed-math", NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Build failed! Log:\n");
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = (char*)malloc(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("%s\n", log);
        free(log);
        exit(1);
    }

    // Create kernel
    kernel = clCreateKernel(program, "matmul", &err);
    CHECK_ERROR(err, "Failed to create kernel");

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &M);
    err |= clSetKernelArg(kernel, 4, sizeof(int), &N);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &K);
    CHECK_ERROR(err, "Failed to set kernel args");

    // Execute kernel
    size_t global[2] = {(M + TS - 1) / TS * TS, (N + TS - 1) / TS * TS};
    size_t local[2] = {TS, TS};

    printf("Launching kernel with global size [%zu,%zu], local [%zu,%zu]\n", 
           global[0], global[1], local[0], local[1]);

    cl_event kernel_event;
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, &kernel_event);
    CHECK_ERROR(err, "Failed to enqueue kernel");

    clFinish(queue);

    cl_ulong start, end;
    clGetEventInfo(kernel_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventInfo(kernel_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    double time_ms = (end - start) * 1.0e-6;

    err = clEnqueueReadBuffer(queue, d_C, CL_TRUE, 0, M * N * sizeof(float), C_gpu, 0, NULL, NULL);
    CHECK_ERROR(err, "Failed to read buffer");

    printf("OpenCL Matrix Multiply completed in %.3f ms\n", time_ms);
    printf("Performance: %.2f GFLOPS\n", 2.0 * M * N * K / (time_ms * 1e6));

    // CPU verification (first 10x10 block)
    printf("\nVerifying result (top-left 10x10)... ");
    for (int i = 0; i < 10 && i < M; i++) {
        for (int j = 0; j < 10 && j < N; j++) {
            float sum = 0;
            for (int k = 0; k < K; k++) {
                sum += A[i * K + k] * B[k * N + j];
            }
            C_host[i * N + j] = sum;
        }
    }

    int errors = 0;
    for (int i = 0; i < 10 && i < M; i++) {
        for (int j = 0; j < 10 && j < N; j++) {
            if (fabs(C_host[i*N+j] - C_gpu[i*N+j]) > 1e-3) {
                errors++;
            }
        }
    }
    printf("%s\n", errors == 0 ? "PASSED ✓" : "FAILED ✗");

    clReleaseEvent(kernel_event);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(d_A);
    clReleaseMemObject(d_B);
    clReleaseMemObject(d_C);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(A); free(B); free(C_host); free(C_gpu);

    return 0;
}