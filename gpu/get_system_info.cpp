#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <CL/opencl.hpp>
#include <iostream>

#include "opencl_helpers.cpp"

int main(int argc, char *argv[]) {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    bool verbose = (argc > 1 && std::string(argv[1]) == "-v");

    std::string ident = "    ";
    for (auto &platform : platforms) {
        std::cout << "Found platform: " << platform << " with devices:" << std::endl;

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        for (auto &device : devices) {
            std::cout << ident << device;
            if (verbose) std::cout << " with extensions: ";
            std::cout << std::endl;

            if (!verbose) continue;
            std::vector<std::string> extensions = get_device_extensions(device);
            for (auto &extension : extensions) {
                std::cout << ident << ident << extension << std::endl;
            }

            std::cout << std::endl;
            if (std::find(extensions.begin(), extensions.end(), "cl_khr_il_program") !=
                extensions.end()) {
                std::cout << ident << ident
                          << "cl_khr_il_program extension is supported\n";
            } else {
                std::cout << ident << ident
                          << "cl_khr_il_program extension is not supported\n";
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
    }
    return 0;
}
