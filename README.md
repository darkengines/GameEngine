# GameEngine

## How to build

Install the [Vulkan SDK](https://vulkan.lunarg.com/).  
Set **VULKAN_SDK** environment variable to the vulkan SDK root directory path.  
  
Install [vcpkg](https://vcpkg.io/en/getting-started.html).  
Set **VCPKG_ROOT** environment variable to the vcpkg root directory path.  
Add **VCPKG_ROOT**/Bin to your path environment variable (required to call glslc).
  
Install [CMake](https://cmake.org/).  
Run 
```sh
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
cmake --build build
```
