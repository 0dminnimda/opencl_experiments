import sys
from pathlib import Path
from subprocess import call


SAN = True
DEBUG = True

OPENCL_PATH = Path(r"C:\OpenCL-SDK\v2023.04.17-Win-x64")


def build():
    program = sys.argv[1]
    args = [program, "-o", "main.out"]

    libs = []
    includes = []

    libs.append((OPENCL_PATH / "lib"))
    includes.append((OPENCL_PATH / "include"))

    is_cpp = program.endswith("cpp")
    if is_cpp:
        args.extend(["--std=c++17"])
    args.extend(["-Wno-deprecated-declarations", "-lOpenCL"])

    if SAN:
        args.append("-fsanitize=undefined,address")

    if DEBUG:
        args.append("-O0")
        args.append("-g")

    for lib in libs:
        args.append("-L")
        args.append(str(lib))

    for inc in includes:
        args.append("-I")
        args.append(str(inc))

    return call(["clang"] + args)


print("Return code:", build())

# also SEE:
# - https://thrust.github.io/
# - https://www.openacc.org/
# - http://boostorg.github.io/compute/
# - https://github.com/HSA-Libraries/Bolt
# - https://arrayfire.com/
# - https://skelcl.github.io/
# - https://www.khronos.org/sycl/

# https://github.com/KhronosGroup/OpenCL-Guide/blob/main/chapters/cpp_for_opencl.md
# https://forums.developer.nvidia.com/t/clcreateprogramwithil-alternatives/216557
# https://github.com/KhronosGroup/OpenCL-Guide/blob/main/chapters/os_tooling.md
# https://www.khronos.org/blog/offline-compilation-of-opencl-kernels-into-spir-v-using-open-source-tooling
