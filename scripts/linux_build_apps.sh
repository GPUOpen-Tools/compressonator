cd ..
export VULKAN_SDK=~/vulkan/1.2.141.2
QT_ROOT=~/Qt5.12.6/5.12.6/gcc_64
cmake -DQT_PACKAGE_ROOT=$QT_ROOT .
make

