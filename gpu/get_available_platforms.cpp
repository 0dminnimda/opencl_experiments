#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <CL/opencl.hpp>
#include <iostream>

#include "opencl_helpers.cpp"

int main() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    for (auto &platform : platforms) {
        std::string platform_version = platform.getInfo<CL_PLATFORM_VERSION>();
        std::cout << "Found platform: '" << platform_version << "' (version "
                  << as_opencl_version(platform_version)
                  << ") with devices:" << std::endl;

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        for (auto &device : devices) {
            std::cout << "    " << device.getInfo<CL_DEVICE_NAME>() << " ("
                      << device_type_string(device.getInfo<CL_DEVICE_TYPE>()) << ")"
                      << std::endl;
        }

        std::cout << std::endl;
    }
    return 0;
}
