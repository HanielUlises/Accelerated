__kernel void nonlinear_transposed_fused_matmul(__global const float* x, __global const float* y, __global float* z, int n) {
    int gx = get_global_id(0);
    int gy = get_global_id(1);
    int lx = get_local_id(0);
    int ly = get_local_id(1);

    __local float tile_x[32][33];
    __local float tile_y[32][33];

    float acc = 0.0f;

    for (int t = 0; t < n; t += 32) {
        tile_x[ly][lx] = (gx < n && t + lx < n) ? x[gy * n + (t + lx)] : 0.0f;
        tile_y[ly][lx] = (gy < n && t + ly < n) ? y[(t + ly) * n + gx] : 0.0f;

        barrier(CLK_LOCAL_MEM_FENCE);

        #pragma unroll 8
        for (int k = 0; k < 32 && t + k < n; ++k) {
            float xv = tile_x[ly][k];
            float yv = tile_y[k][lx];
            acc += xv * yv + native_sin(xv) * native_cos(yv + xv * 0.1f);
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    __local float partial[256];
    int lid = ly * get_local_size(0) + lx;
    partial[lid] = acc;

    barrier(CLK_LOCAL_MEM_FENCE);

    for (int s = 128; s > 0; s >>= 1) {
        if (lid < s) partial[lid] += partial[lid + s] * 1.05f;
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (lid == 0 && gx < n && gy < n)
        z[gy * n + gx] = partial[0] * native_rsqrt((float)n);
}