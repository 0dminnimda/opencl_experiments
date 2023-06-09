#define CL_HPP_TARGET_OPENCL_VERSION 300

#include <iostream>
#include <vector>

#include <CL/opencl.hpp>

int main() {
  printf("Hello World!\n");

  // // Get available platforms
  // std::vector<cl::Platform> platforms;
  // cl::Platform::get(&platforms);

  // // Get available devices
  // std::vector<cl::Device> devices;
  // platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

  // // Create a context and command queue
  // cl::Context context(devices);
  // cl::CommandQueue queue(context, devices[0]);

  // std::cout << "OpenCL device: " << devices[0].getInfo<CL_DEVICE_NAME>()
  //           << std::endl;

  // Get available platforms
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  // Get available devices
  std::vector<cl::Device> devices;
  platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

  // Create a context and command queue
  cl::Context context(devices);
  cl::CommandQueue queue(context, devices[0]);

  std::ifstream kernelFile("kernel.cl");
  std::string kernelSource((std::istreambuf_iterator<char>(kernelFile)),
                           std::istreambuf_iterator<char>());
  cl::Program::Sources sources(
      1, std::make_pair(kernelSource.c_str(), kernelSource.length() + 1));
  cl::Program program(context, sources);

  // Build the program
  program.build(devices);

  // Create an input buffer
  cl::Buffer inputBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * size);

  // Create an output buffer
  cl::Buffer outputBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * size);

  kernel.setArg(0, inputBuffer);
  kernel.setArg(1, outputBuffer);
  kernel.setArg(2, size);

  // Enqueue the kernel
  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(size),
                             cl::NullRange);

  // Wait for the kernel to finish
  queue.finish();

  // Read the output buffer
  queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(float) * size,
                          outputData);

  return 0;
}
