#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <CL/opencl.hpp>
#include <iostream>

#include "opencl_helpers.cpp"

int main() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    for (auto &platform : platforms) {
        std::cout << "Found platform: " << platform << " with devices:" << std::endl;

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        for (auto &device : devices) { std::cout << "    " << device << std::endl; }

        std::cout << std::endl;
    }
    return 0;
}
