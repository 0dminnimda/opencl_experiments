
kernel void vector_add(global float *A,
                       global float *B,
                       global float *C) {
    int gid = get_global_id(0);   
    C[gid] = A[gid] + B[gid] / 2;
    if (gid == 0) printf("%p %p\n", A, B);
    A[gid] = 1;
    B[gid] = 1;
}
