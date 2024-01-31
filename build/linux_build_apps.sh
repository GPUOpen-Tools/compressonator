set -x

# This is done to ensure the script works whether WORKSPACE is set or not
# We assume this script is called from the root "compressonator" folder
if [ -z "$WORKSPACE" ] 
then
    WORKSPACE=".."
fi

cd $WORKSPACE/compressonator/build
python3 fetch_dependencies.py
cd ..

cd $WORKSPACE/compressonator

export VULKAN_DIR=/opt/VulkanSDK/1.2.141.2/
export QT_DIR=/opt/Qt/Qt5.9.2/5.9.2/gcc_64/
export OPENCV_DIR=/opt/opencv/

cmake .
make

