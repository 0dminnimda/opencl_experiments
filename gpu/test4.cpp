#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <CL/opencl.hpp>
#include <cstdio>
#include <iostream>
#include <vector>

#include "opencl_helpers.cpp"

static const std::string kernel_source = R"(
__kernel void vector_add(__global float *A,
                         __global float *B,
                         __global float *C) {
    int gid = get_global_id(0);   
    C[gid] = A[gid] + B[gid];
    printf("%p %p\n", A, B);
    A[gid] = 1;
    B[gid] = 1;
}
)";

int main() {
    cl::Device device;
    if (get_gpu_device(device)) { return -1; }
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    const int N = 50;

    std::vector<float> A(N);
    std::vector<float> B(N);
    std::vector<float> C(A.size());
    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = N - i;
    }

    A[2] += 0.2;
    B[4] += 0.8;

    for (size_t i = 0; i < C.size(); ++i) {
        std::cout << "A[" << i << "] = " << A[i] << std::endl;
    }
    for (size_t i = 0; i < C.size(); ++i) {
        std::cout << "B[" << i << "] = " << B[i] << std::endl;
    }
    for (size_t i = 0; i < C.size(); ++i) {
        std::cout << "C[" << i << "] = " << C[i] << std::endl;
    }

    cl::Buffer bufferA(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                       sizeof(float) * A.size(), A.data());
    cl::Buffer bufferB(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                       sizeof(float) * B.size(), B.data());
    cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, sizeof(float) * C.size());

    // see the adresses of the data and the buffers, to compare to the kernel ones
    printf("%p %p\n", A.data(), B.data());

    void *devicePtr1 = nullptr;
    bufferA.getInfo(CL_MEM_HOST_PTR, &devicePtr1);
    void *devicePtr2 = nullptr;
    bufferB.getInfo(CL_MEM_HOST_PTR, &devicePtr2);
    printf("%p %p\n", devicePtr1, devicePtr2);

    // Build program
    cl::Program::Sources sources({kernel_source});
    cl_int err;
    cl::Program program(context, sources);
    err = program.build(device);
    if (err != CL_SUCCESS) {
        std::cout << "Error building program: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        return 1;
    }

    // Create kernel
    cl::Kernel kernel(program, "vector_add", &err);
    if (err != CL_SUCCESS) {
        std::cout << "Error creating kernel: " << err << std::endl;
        return 1;
    }

    // Set kernel arguments
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferC);

    // Execute kernel
    cl::NDRange globalSize(A.size());
    cl::NDRange localSize(1);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, localSize);

    queue.finish();

    // Read results from the output buffer
    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, sizeof(float) * C.size(), C.data());

    // Print the result
    for (size_t i = 0; i < C.size(); ++i) {
        std::cout << "A[" << i << "] = " << A[i] << std::endl;
    }
    for (size_t i = 0; i < C.size(); ++i) {
        std::cout << "B[" << i << "] = " << B[i] << std::endl;
    }
    for (size_t i = 0; i < C.size(); ++i) {
        std::cout << "C[" << i << "] = " << C[i] << std::endl;
    }

    return 0;
}
