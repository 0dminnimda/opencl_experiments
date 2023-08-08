// #define CL_MINIMUM_OPENCL_VERSION 120
// #define CL_TARGET_OPENCL_VERSION 120

#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)

int main() {
    cl_int err;

    // Load the SPIR-V binary from file
    FILE *fp;
    char *binary;
    size_t binary_size;

    fp = fopen("test.spv", "rb");
    if (!fp) {
        printf("Failed to open SPIR-V binary file.\n");
        exit(1);
    }

    binary = (char*)malloc(MAX_SOURCE_SIZE);
    binary_size = fread(binary, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    printf("Successfully loaded SPIR-V binary file.\n");

    // Create OpenCL context
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;

    err = clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

    printf("Loading the program.\n");

    // Create OpenCL program from SPIR-V binary
    cl_program program = clCreateProgramWithIL(context, binary, binary_size, &err);

    printf("Building the program.\n");

    // Build the OpenCL program
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // Do something with the OpenCL program...

    // Cleanup
    clReleaseProgram(program);
    clReleaseContext(context);
    free(binary);

    return 0;
}
