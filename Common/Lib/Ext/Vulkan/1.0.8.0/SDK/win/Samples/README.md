# Vulkan Samples Kit
  - This repository is a collection of Vulkan C++ sample applications.
  - Run the following script to obtain a short description of all or a 
    specific sample:
    `$ src/get-short-descripts.sh`
  - Run the following script to obtain a more detailed description of all
    samples with a long description set:
    `$ src/get-descripts.sh`

## Requirements/Dependencies
  - A recent version of the LunarG Vulkan SDK must be installed on the system.
    In addition to the Vulkan headers and libraries, the SDK also includes the
    glslang and spirv-tools components necessary to build the sample progams.
  - Linux package dependencies:
    - Vulkan SDK required packages
    - cmake
  - Windows dependencies include:
    - Vulkan SDK required components
    - cmake

## Kit Structure
  - The Vulkan Samples Kit is a set of source and data files in a specific
    directory hierarchy:
      - API-Samples - Samples that demonstrate the use of various aspects of the
        Vulkan API
      - Demos - Samples that are more functional and go deeper than simple API use.
      - Layer-Samples - Samples that are implemented as layers.
      - Android-Only - Samples that are only meant to be run on Android

## Sample progression
  - In general, the samples are not interrelated, but there is a progression
      among some of the samples that lead to drawing a cube.  Start with the
      instance sample, then device, initcommandbuffer, initswapchain, initdepthbuffer,
      inituniformbuffer, descriptor_pipeline_layouts, initrenderpass, initshaders,
      initframebuffers, vertexbuffer, allocdescriptorsets, initpipeline, and they
      culminate in the drawcube sample.  Each sample uses utility routines from
      the code from previous samples to get to the point to show something new.
      The drawtexturedcube sample takes all of the drawcube code and adds texturing.

## Building the Vulkan Samples Kit
### Linux:
  ```
  $ cmake -H. -Bbuild
  $ make -C build 
  ```

### Windows:
#### 64-bit Windows

From a PowerShell window, first build glslang:
  ```
  PS > cd C:\VulkanSDK\<version>\glslang
  PS > mkdir build
  PS > cd build
  PS > cmake -G "Visual Studio 12 Win64" ..
  ```
  Open the glslang.sln file in the build folder with Microsoft Visual Studio and build the solution in both Debug and Release mode.

Next build spirv-tools:
  ```
  PS > cd C:\VulkanSDK\<version>\spirv-tools
  PS > mkdir build
  PS > cd build
  PS > cmake -G "Visual Studio 12 Win64" ..
  ```

  Open the spirv-tools.sln file in the build folder with Microsoft Visual Studio and build the solution in both Debug and Release mode.

Next build the Vulkan Samples:
  ```
  PS > cd C:\VulkanSDK\<version>\Samples
  PS > mkdir build
  PS > cd build
  PS > cmake -G "Visual Studio 12 Win64" ..
  ```
  Open the VULKAN_SAMPLES.sln file in the build folder with Microsoft Visual Studio and build the solution.
  
#### 32-bit Windows

Perform the same operations as 64-bit Windows above, except with the following adjustments:
  ```
  PS > mkdir build32
  PS > cd build32
  PS > cmake -G "Visual Studio 12" ..
  ```
  
## Contributing
  Refer to the README.contrib file for specific info regarding contributing to
  the Vulkan samples creation effort.

