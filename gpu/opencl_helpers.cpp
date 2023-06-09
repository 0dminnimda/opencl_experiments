#include <CL/opencl.hpp>
#include <cctype>
#include <iostream>
#include <string>

std::string as_platform_version(int opencl_version) {
    return "OpenCL " + std::to_string(opencl_version / 100) + "." +
           std::to_string((opencl_version % 100) / 10);
}

int as_opencl_version(const std::string &platform_version, bool check_format = true) {
    // format: "OpenCL X.X" + allowed garbage at the end

    std::string prefix = "OpenCL ";
    std::size_t offset = prefix.size();

    if (check_format && platform_version.compare(0, offset, prefix) != 0) { return -1; }
    if (check_format && platform_version[offset + 1] != '.') { return -1; }

    int major_version = platform_version[offset] - '0';
    int minor_version = platform_version[offset + 2] - '0';

    if (major_version < 0 || minor_version < 0) { return -1; }
    return major_version * 100 + minor_version * 10;
}

bool platform_version_matches(const cl::Platform &platform,
                              const std::string &wanted_version,
                              std::string &current_version) {
    current_version = platform.getInfo<CL_PLATFORM_VERSION>();
    return current_version.find(wanted_version) != std::string::npos;
}

bool platform_version_matches(const cl::Platform &platform, const std::string &version) {
    std::string current_version;
    return platform_version_matches(platform, version, current_version);
}

std::string device_type_string(cl_device_type type) {
    if (type == CL_DEVICE_TYPE_DEFAULT) {
        return "default";
    } else if (type == CL_DEVICE_TYPE_CPU) {
        return "cpu";
    } else if (type == CL_DEVICE_TYPE_GPU) {
        return "gpu";
    } else if (type == CL_DEVICE_TYPE_ACCELERATOR) {
        return "accelerator";
    }
#ifdef CL_DEVICE_TYPE_CUSTOM
    else if (type == CL_DEVICE_TYPE_CUSTOM) {
        return "custom";
    }
#endif
    return "unknown";
}

std::ostream &operator<<(std::ostream &stream, const cl::Platform &platform) {
    std::string platform_version = platform.getInfo<CL_PLATFORM_VERSION>();
    stream << platform_version;

    // "OpenCL 1.2 " is possible value of platform_version
    if (!std::isspace(platform_version[platform_version.size() - 1])) stream << " ";

    stream << "(version " << as_opencl_version(platform_version) << ")";
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const cl::Device &device) {
    stream << device.getInfo<CL_DEVICE_NAME>() << " ("
           << device_type_string(device.getInfo<CL_DEVICE_TYPE>()) << ")";
    return stream;
}

std::vector<cl::Device> get_suitable_devices(const std::string &platform_version,
                                             cl_device_type type) {
    std::vector<cl::Device> devices;

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    for (auto &platform : platforms) {
        if (!platform_version_matches(platform, platform_version)) continue;

        std::vector<cl::Device> current_devices;
        platform.getDevices(type, &current_devices);
        devices.insert(devices.end(), current_devices.begin(), current_devices.end());
    }

    return devices;
}

bool get_gpu_device(cl::Device &device) {
    std::string platform_version = as_platform_version(CL_HPP_TARGET_OPENCL_VERSION);

    // cl_int error = CL_SUCCESS;
    // cl::Platform platform = cl::Platform::getDefault(&error);
    // if (error == CL_SUCCESS &&
    //     platform_version_matches(platform, target_version)) {
    //     return platform;
    // }

    std::vector<cl::Device> devices =
        get_suitable_devices(platform_version, CL_DEVICE_TYPE_GPU);
    if (devices.size() == 0) {
        std::cout << "No GPU device found, that is compatible with " << platform_version
                  << "\n";
        return true;
    }
    std::cout << "Available devices:\n";
    for (auto &device : devices) {
        cl::Platform platform(device.getInfo<CL_DEVICE_PLATFORM>(), true);
        std::cout << "  " << platform << " - " << device << std::endl;
    }
    std::cout << "\n";
    device = devices[0];

    return false;
}
