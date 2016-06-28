# Be sure to run "Set-ExecutionPolicy RemoteSigned" before running powershell scripts

Param([switch]$Debug)

if ($Debug) {
    $dPath = "Debug"
    } else {
    $dPath = "Release"
}

echo "Instance"
& $dPath\instance
echo "Instance Layer Properties"
& $dPath\instance_layer_properties
echo "Instance Layer Properties"
& $dPath\instance_layer_properties
echo "Instance Extension Properties"
& $dPath\instance_extension_properties
echo "Instance Layer ExtensionProperties"
& $dPath\instance_layer_extension_properties
echo "Enumerate"
& $dPath\enumerate
echo "Device"
& $dPath\device
echo "Enable Validation and Debug Message Callback"
& $dPath\enable_validation_with_callback
echo "Create Debug Message Callback"
& $dPath\dbgcreatemsgcallback
echo "Init Swapchain"
& $dPath\initswapchain
echo "Init Command Buffer"
& $dPath\initcommandbuffer
echo "Depth Buffer"
& $dPath\depthbuffer
echo "Init Texture"
& $dPath\inittexture
echo "Uniform Buffer"
& $dPath\uniformbuffer
echo "Init RenderPass"
& $dPath\initrenderpass
echo "Init Framebuffer"
& $dPath\initframebuffers
echo "Descriptor and Pipeline Layouts"
& $dPath\descriptor_pipeline_layouts
echo "Vertex Buffer"
& $dPath\vertexbuffer
echo "Alloc Descriptor Sets"
& $dPath\allocdescriptorsets
echo "Initialize Shaders"
& $dPath\initshaders
echo "Initialize Pipeline"
& $dPath\initpipeline
echo "Draw Cube"
& $dPath\drawcube
echo "Copy/Blit Image"
& $dPath\copyblitimage
echo "Draw Textured Cube"
& $dPath\drawtexturedcube
echo "Draw Cubes with Dynamic Uniform Buffer"
& $dPath\dynamicuniform
echo "Texel Buffer"
& $dPath\texelbuffer
echo "Immutable Sampler"
& $dPath\immutable_sampler
echo "Multiple Descriptor Sets"
& $dPath\multiple_sets
echo "Multithreaded Command Buffers"
& $dPath\multithreadcmdbuf
echo "Push Constants"
& $dPath\push_constants
echo "Separate image sampler"
& $dPath\separate_image_sampler
echo "Draw Sub-passes"
& $dPath\drawsubpasses
echo "Occlusion Query"
& $dPath\occlusion_query
echo "Pipeline Cache"
& $dPath\pipeline_cache
echo "Pipeline Derivative"
& $dPath\pipeline_derivative
echo "Secondary Command Buffers"
& $dPath\secondarycmd
echo "SPIR-V Assembly"
& $dPath\spirv_assembly
echo "SPIR-V Specialization"
& $dPath\spirv_specialization
