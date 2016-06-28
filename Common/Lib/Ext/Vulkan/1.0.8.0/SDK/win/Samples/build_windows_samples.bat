cd %VULKAN_SDK%
cd glslang
md build
cd build
cmake -G "Visual Studio 12 Win64" ..
msbuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Debug /verbosity:quiet
msbuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Release /verbosity:quiet
cd ..\..
cd spirv-tools
md build
cd build
cmake -G "Visual Studio 12 Win64" ..
msbuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Debug /verbosity:quiet
msbuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Release /verbosity:quiet
cd ..\..
cd samples
md build
cd build
cmake -G "Visual Studio 12 Win64" ..
msbuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Debug /verbosity:quiet
msbuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Release /verbosity:quiet

