#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <CL/opencl.hpp>
#include <iostream>
#include <vector>

#include "opencl_helpers.cpp"

int main() {
    // Define input data
    std::vector<float> A = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    std::vector<float> B = {5.5f, 4.0f, 3.0f, 2.3f, 1.0f};
    std::vector<float> C(A.size());

    // Get default platform and device
    cl::Device device;
    if (get_gpu_device(device)) { return -1; }

    // Create context and command queue
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    // Create buffers
    cl::Buffer bufferA(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       sizeof(float) * A.size(), A.data());
    cl::Buffer bufferB(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       sizeof(float) * B.size(), B.data());
    cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, sizeof(float) * C.size());

    // Kernel source code
    // const char *kernelSource =
    // "__kernel void vector_add(__global const float *A, __global const float *B, "
    // "__global float *C) {"
    // "  int gid = get_global_id(0);"
    // "  C[gid] = A[gid] + B[gid];"
    // "}";
    const char *kernelSource =
        "__kernel void vector_add(__global const float *A, __global const float *B, "
        "__global float *C) {"
        "  int gid = get_global_id(0);"
        "  C[gid] = A[gid] + B[gid];"
        "}";

    // Build kernel
    cl::Program::Sources sources({kernelSource});
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

    // Read results from the output buffer
    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, sizeof(float) * C.size(), C.data());

    // Print the result
    for (size_t i = 0; i < C.size(); ++i) {
        std::cout << "C[" << i << "] = " << C[i] << std::endl;
    }

    return 0;
}
