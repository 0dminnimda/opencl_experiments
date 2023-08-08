#include <CL/sycl.hpp>
#include <iostream>

int main() {
    auto platforms = cl::sycl::platform::get_platforms();
    for (auto& p : platforms) {
        std::cout << p.get_info<sycl::info::platform::version>() << std::endl;
        std::cout << p.get_info<sycl::info::platform::name>() << std::endl;
        std::cout << p.get_info<sycl::info::platform::vendor>() << std::endl;
        auto devs = p.get_devices();
        std::cout << "Platform has: " << devs.size() << " devices\n";
        for (auto& d : devs) {
            std::cout << "\t" << d.get_info<sycl::info::device::name>() << std::endl;
        }
    }
    auto platform = platforms.at(0);
    std::cout << "End1\n";
    auto ctx = cl::sycl::context{platform};
    std::cout << "End2\n";
    auto dev = ctx.get_devices().at(0);
    std::cout << "End3\n";
    auto queue = cl::sycl::queue{dev};
    std::cout << "End\n";
    return 0;
}


// #include <sycl/sycl.hpp>

// int CUDASelector(const sycl::device &Device) {
//   using namespace sycl::info;
//   const std::string DriverVersion = Device.get_info<device::driver_version>();

//   if (Device.is_gpu() && (DriverVersion.find("CUDA") != std::string::npos)) {
//     std::cout << " CUDA device found " << std::endl;
//     return 1;
//   };
//   return -1;
// }

// int main() {

//   auto NEOGPUDeviceSelector = [](const sycl::device &Device){
//     using namespace sycl::info;

//     const std::string DeviceName = Device.get_info<device::name>();
//     bool match = Device.is_gpu() && (DeviceName.find("HD Graphics NEO") != std::string::npos);
//     return match ? 1 : -1;
//   };

//   try {
//     sycl::queue Queue(NEOGPUDeviceSelector);
//     sycl::device Device(NEOGPUDeviceSelector);
//   } catch (sycl::exception &E) {
//     std::cout << E.what() << std::endl;
//   }
// }
