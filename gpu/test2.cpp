#define CL_HPP_TARGET_OPENCL_VERSION 300

#include <CL/opencl.hpp>
#include <iostream>
#include <vector>

static const std::string gpuCode = R"(
__kernel void addVectors(__global const int* A,
                         __global const int* B,
                         __global int* C) {
    int i = get_global_id(0);
    C[i] = A[i] + B[i];
}
)";

int main() {
    // Create two vectors to be added
    std::vector<int> A = {1, 2, 3, 4, 5};
    std::vector<int> B = {6, 7, 8, 9, 10};

    // Get available OpenCL platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // Choose the first platform and create a context
    cl::Context context(CL_DEVICE_TYPE_GPU);
    std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

    // Create a command queue for the first device
    cl::CommandQueue queue(context, devices[0]);

    // Create OpenCL buffers for the input and output vectors
    cl::Buffer bufferA(context, CL_MEM_READ_ONLY, A.size() * sizeof(int));
    cl::Buffer bufferB(context, CL_MEM_READ_ONLY, B.size() * sizeof(int));
    cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, A.size() * sizeof(int));

    // Copy the input vectors to the buffers
    queue.enqueueWriteBuffer(bufferA, /* blocking = */ CL_TRUE, 0, A.size() * sizeof(int),
                             A.data());
    queue.enqueueWriteBuffer(bufferB, /* blocking = */ CL_TRUE, 0, B.size() * sizeof(int),
                             B.data());

    // Create an OpenCL program from the source code
    std::string sourceCode = gpuCode;
#if 0
    cl::Program::Sources sources(1, std::make_pair(sourceCode.c_str(), sourceCode.length()));
#elif 0
    std::vector<std::string> sources = {sourceCode};
    cl::Program::Sources sources(sources);
#else
    cl::Program::Sources sources = {sourceCode};
#endif
    cl::Program program(context, sources);

    // Build the program for the devices
    program.build(devices);

    // Create a kernel from the program
    cl::Kernel kernel(program, "addVectors");

    // Set kernel arguments
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferC);

    // Enqueue the kernel with the global size equal to the size of the vectors
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(A.size()),
                               cl::NullRange);

    // Wait for all commands to complete
    queue.finish();

    // Read the output vector from the buffer
    std::vector<int> C(A.size());
    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, C.size() * sizeof(int), C.data());

    // Print the result
    for (int i = 0; i < C.size(); i++) { std::cout << C[i] << " "; }
    std::cout << std::endl;

    return 0;
}
