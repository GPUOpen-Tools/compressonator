# Compressonator

Compressonator is a set of tools to allow artists and developers to more easily create compressed texture assets or model mesh optimizations and easily visualize the quality impact of various compression and rendering technologies.  It consists of a GUI application, a command line application and an SDK for easy integration into a developer tool chain.

Compressonator supports Microsoft Windows®, Linux and Mac builds.

For more details goto the online Compressonator Documents: http://compressonator.readthedocs.io/en/latest/ 

Get Prebuilt Binaries and Installer here:
---------------------------------------------------
https://github.com/GPUOpen-Tools/Compressonator/releases

To build the source files follow the instructions in http://compressonator.readthedocs.io/en/latest/build_from_source/build_instructions.html

## Contributors

Compressonator's GitHub repository (http://github.com/GPUOpen-Tools/Compressonator) is moderated by Advanced Micro Devices, Inc. as part of the GPUOpen initiative.

AMD encourages any and all contributors to submit changes, features, and bug fixes via Git pull requests to this repository.

Users are also encouraged to submit issues and feature requests via the repository's issue tracker.
  
Under development 
----------------------------------------------------
* Radeon Powered Compression with Compute and Packed Math Libraries for CPU and GPU

  Feature info for optimal performance on Ryzen™ with Radeon™ RX Vega Graphics:
  * SenseMI  https://www.amd.com/en/technologies/sense-mi
  * Desktop PC Zen Core  https://www.amd.com/en/technologies/zen-core

* Improvements to existing Compression Codecs
  * Fix for R & B channel swizzling
  * updated ETC Codec with support for SRGB, EAC and punchthrough Alpha
    
* Selectable 3D Model viewers
  * Support for the Vulkan® and OpenGL® Viewer code introduced in 3.0
 
* 3D Model Image Diff viewer 
  * Selectable animation rates 
  * Manual mode to flip views
 

Source Code changes since 3.0 release
------------------------------------------------------

* CLI support for Model Mesh Compression and Model Optimization

* Model Mesh Compression 
  * Support for GLTF 2.0 compression extensions (KHR_draco_mesh_compression)
  
* BC6H improvements
  * Improved quality for light map textures
  * Fix BC6H delta endpoints bit overflow with signed in single region
  * Add endpoints error checking for BC6H one region modes
  


Recent 3.0 additions and changes
------------------------------------------------------

* Model Mesh Optimization for OBJ and GLTF 2.0 files
  * 3D triangle mesh optimization that improves on existing mesh preprocessing techniques
    * Vertex cache optimization
    * Overdraw optimization
    * Vertex prefetch cache optimization
    
* Model Mesh Compression for OBJ files 
  * Compresss models for faster download using Draco .drc files
    * Set quantization bits for POSITION, NORMAL, TEX_CORD and other GENERIC attributes
  
* DirectX® 12 3D Model viewer features and modifications
  * Multi GPU Node usage stats
  * Real time GPU profiler chart with Total GPU Time stats
  * Animation (on/off)
  * Glow
  * Spot lighting 
  * Shadow maps
  * Selection of tone mapper ("Timothy", "DX11DSK", "Reinhard", "Uncharted2Tonemap")
  * Exposure setting (0.0 to 1.0)
  * iblFactor (0.0 to 2.0)
  * Show Bounding Boxes
  * Show Sky dome
 
* 2D Image viewer update
  * Switch overlaid views of processed (compressed), Original and or Pixel Image Differance views with simple key stokes (O, P or D) and toggle the views at any time from Origin to Processed or Original to Image Diff using space bar

Gallery
-------
**2D Texture Compression**

![screenshot 1](https://github.com/GPUOpen-Tools/Compressonator/blob/master/docs/source/gui_tool/user_guide/media/image51.png)

**GLTF 2.0 Model PBR Texture Compression**

![screenshot 2](https://github.com/GPUOpen-Tools/Compressonator/blob/master/docs/source/gui_tool/user_guide/media/image96.png)


