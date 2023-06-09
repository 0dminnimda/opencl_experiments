#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.hpp>
#include <iostream>

int main() {
    const int N = 1024;
    int A[N], B[N], C[N];

    // Initialize input arrays
    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = N - i;
    }

    // Get available platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // Get available devices
    std::vector<cl::Device> devices;
    platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

    // Create context and command queue
    cl::Context context(devices);
    cl::CommandQueue queue(context, devices[0]);

    // Create input and output buffers
    cl::Buffer bufferA(context, CL_MEM_READ_ONLY, N * sizeof(int));
    cl::Buffer bufferB(context, CL_MEM_READ_ONLY, N * sizeof(int));
    cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, N * sizeof(int));

    // Copy input data to buffers
    queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, N * sizeof(int), A);
    queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, N * sizeof(int), B);

    // Define kernel code
    const char* source = R"(
        void kernel add_arrays(global const int* A, global const int* B, global int* C) {
            int i = get_global_id(0);
            C[i] = A[i] + B[i];
        }
    )";

    // Create program and kernel
    cl::Program program(context, cl::Program::Sources({source}));
    program.build(devices);
    cl::Kernel kernel(program, "add_arrays");

    // Set kernel arguments
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferC);

    // Execute kernel
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(N), cl::NullRange);

    // Copy output data from buffer
    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, N * sizeof(int), C);

    // Print result
    for (int i = 0; i < N; i++) {
        std::cout << C[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
