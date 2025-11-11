__kernel void vector_add(__global int *C, __global int *A, __global int *B, int numElements) {
    int gid = get_global_id(0);

    if (gid < numElements) {
        C[gid] = A[gid] + B[gid];
    }
}