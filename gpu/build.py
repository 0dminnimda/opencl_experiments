import sys
from pathlib import Path
from subprocess import call


SAN = False
DEBUG = True

OPENCL_PATH = Path(r"C:\OpenCL-SDK\v2023.04.17-Win-x64")


def build():
    program = sys.argv[1]
    args = [program, "-o", "main.out"]

    libs = []
    includes = []

    libs.append((OPENCL_PATH / "lib"))
    includes.append((OPENCL_PATH / "include"))

    args.extend(["--std=c++17", "-Wno-deprecated-declarations", "-lOpenCL"])

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
