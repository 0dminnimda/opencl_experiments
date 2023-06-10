#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120

#include <iostream>
#include <CL/opencl.hpp>

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int main() {
    // Create a context and command queue
    cl::Context context(CL_DEVICE_TYPE_GPU);
    cl::CommandQueue queue(context);

    // Define the function object type
    typedef std::function<int(int, int)> func_type;

    // Create a buffer to hold the function object
    cl::Buffer func_buffer(context, CL_MEM_READ_ONLY, sizeof(func_type));

    // Set the function object value
    func_type func = &add;
    queue.enqueueWriteBuffer(func_buffer, CL_TRUE, 0, sizeof(func_type), &func);

    // Define the kernel code
    const char* kernel_source =
        "#include <functional>\n"
        "kernel void call_function(global void* func_ptr, global int* a, global int* b) {\n"
        "    int i = get_global_id(0);\n"
        "    int j = get_global_id(1);\n"
        "    int index = i * get_global_size(1) + j;\n"
        "    std::function<int(int, int)> func = *static_cast<std::function<int(int, int)>*>(func_ptr);\n"
        "    func(a[index], b[index]);\n"
        "}\n";

    // Compile the kernel code
    cl::Program::Sources sources = {{kernel_source, strlen(kernel_source)}};
    cl::Program program(context, sources);
    if (program.build() != CL_SUCCESS) {
        std::cout << "Error building program: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault()) << std::endl;
        return 1;
    }

    // Create input buffers
    cl::Buffer a_buffer(context, CL_MEM_READ_ONLY, sizeof(int) * 4);
    cl::Buffer b_buffer(context, CL_MEM_READ_ONLY, sizeof(int) * 4);

    // Set the input buffer values
    int a[4] = {1, 2, 3, 4};
    int b[4] = {5, 6, 7, 8};
    queue.enqueueWriteBuffer(a_buffer, CL_TRUE, 0, sizeof(int) * 4, a);
    queue.enqueueWriteBuffer(b_buffer, CL_TRUE, 0, sizeof(int) * 4, b);

    // Set the kernel arguments
    cl::Kernel kernel(program, "call_function");
    kernel.setArg(0, func_buffer);
    kernel.setArg(1, a_buffer);
    kernel.setArg(2, b_buffer);

    // Launch the kernel
    cl::NDRange global_size(2, 2);
    cl::NDRange local_size(1, 1);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, local_size);

    // Wait for the kernel to finish
    queue.finish();

    std::cout << "bye\n";

    return 0;
}
