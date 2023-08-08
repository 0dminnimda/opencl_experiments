#define CL_HPP_ENABLE_EXCEPTIONS 1
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <CL/opencl.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    cl_int err;

    // Initialize OpenCL context, command queue, and device list
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;

    // ... (code to initialize context, command queue, and device list) ...

    // Create OpenCL program object from source code
    const char *kernel_source = "__kernel void my_kernel() { }";
    cl_program program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error creating program: %d\n", err);
        return 1;
    }

    // Build OpenCL program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error building program: %d\n", err);
        return 1;
    }

    // Retrieve compiled binary from OpenCL program object
    size_t size;
    err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &size, NULL);
    if (err != CL_SUCCESS) {
        printf("Error getting program binary size: %d\n", err);
        return 1;
    }

    unsigned char *binaries = (unsigned char*) malloc(sizeof(unsigned char) * size);
    err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, size, binaries, NULL);
    if (err != CL_SUCCESS) {
        printf("Error getting program binaries: %d\n", err);
        return 1;
    }

    // Write compiled binary to file
    FILE *fp = fopen("my_kernel.bin", "wb");
    fwrite(binaries, sizeof(unsigned char), size, fp);
    fclose(fp);

    // Load compiled binary from file
    fp = fopen("my_kernel.bin", "rb");
    fseek(fp, 0, SEEK_END);
    long lSize = ftell(fp);
    rewind(fp);
    unsigned char *kernel_binary = (unsigned char*) malloc(sizeof(unsigned char)*lSize);
    fread(kernel_binary, sizeof(unsigned char), lSize, fp);
    fclose(fp);

    // Create OpenCL program object from binary
    cl_program binary_program = clCreateProgramWithBinary(context, 1, &device, &size, (const unsigned char **)&kernel_binary, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error creating program from binary: %d\n", err);
        return 1;
    }

    // Build OpenCL program from binary
    err = clBuildProgram(binary_program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error building program from binary: %d\n", err);
        return 1;
    }

    // Clean up
    free(binaries);
    free(kernel_binary);
    clReleaseProgram(program);
    clReleaseProgram(binary_program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
