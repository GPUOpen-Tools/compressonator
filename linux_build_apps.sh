export VULKAN_SDK_VER=1.2.141.2
export VULKAN_SDK=/opt/VulkanSDK/$VULKAN_SDK_VER/
QT_ROOT=/opt/Qt/Qt5.9.2/5.9.2/gcc_64
cmake -DQT_PACKAGE_ROOT=$QT_ROOT .
make

