# GPU

## Setup

To use you need to get the OpenCL SDK from [here]().
It will contian compiled libraries as well as C and C++ API.
For me it's `C/OpenCL-SDK/v2023.04.17-Win-x64`, so I use it, but you should replace it with yours in `.clangd` and `build.py`

You should have gpu(s) and appropriate drivers installed

## Use

Run

```shell
python build.py get_system_info.cpp && ./main.out
```

To see the available platforms and devices + test that everything works.

You also can try

```shell
python build.py test4.cpp && ./main.out
```
