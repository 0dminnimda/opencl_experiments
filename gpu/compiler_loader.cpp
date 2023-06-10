#define CL_HPP_USE_IL_KHR
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <CL/opencl.hpp>
#include <fstream>
#include <iostream>

#include "opencl_helpers.cpp"

// TODO: https://github.com/KhronosGroup/OpenCL-Guide/blob/main/chapters/os_tooling.md

int compile(const std::string &kernel_path, const std::string &binary_path) {
    cl::Device device;
    if (get_gpu_device(device)) { return -1; }
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    std::ifstream input_file(kernel_path);
    std::string source_code((std::istreambuf_iterator<char>(input_file)), {});

    // build for all devices in the context
    cl::Program program(context, source_code);
    if (program.build(device) != CL_SUCCESS) {
        std::cout << "Error building program: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        return 1;
    }

    cl_int error = CL_SUCCESS;
    // std::vector<unsigned char> binary = program.getInfo<CL_PROGRAM_IL_KHR>(&error);
    // std::vector<unsigned char> binary =
    // program.getInfo<CL_PROGRAM_BINARIES>(&error)[0];
    auto binaries = program.getInfo<CL_PROGRAM_BINARIES>(&error);
    if (error != CL_SUCCESS) {
        std::cout << "Error getting binary: " << error << std::endl;
        return 1;
    }

    std::ofstream output_file(binary_path, std::ios::binary);
    output_file.write((const char *)binaries[0].data(), binaries[0].size());
    // for (size_t i = 0; i < binaries.size(); ++i) {
    //     auto binary = binaries[i];
    //     output_file << i << "\n";
    //     output_file.write((const char *)binary.data(), binary.size());
    // }
    output_file.close();

    return 0;
}

int run_compiled(const std::string &filepath) {
    cl::Device device;
    if (get_gpu_device(device)) { return -1; }
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    std::ifstream ilFile(filepath, std::ios::binary);
    if (!ilFile.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    std::vector<char> binary_data(std::istreambuf_iterator<char>(ilFile), {});
    ilFile.close();

    std::cout << "wazzup" << std::endl;

    cl_int error = CL_SUCCESS;
    cl::Program program = cl::Program(context, binary_data, true, &error);
    if (error != CL_SUCCESS) {
        std::cout << "Error building program: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        return 1;
    }

    error = CL_SUCCESS;
    std::string filename = filepath.substr(0, filepath.find_last_of("."));
    std::cout << filename << std::endl;
    return 0;

    cl::Kernel kernel(program, filename.c_str(), &error);
    if (error != CL_SUCCESS) {
        std::cout << "Error creating kernel: " << error << std::endl;
        return 1;
    }
    set_kernel_args(kernel);

    execute_kernel(queue, kernel, cl::NDRange());
    queue.finish();

    return 0;
}

int main() {
    int result = compile("kernel.cl", "kernel.il");
    if (result != 0) { return result; }

    result = run_compiled("kernel.il");
    if (result != 0) { return result; }

    return 0;
}
