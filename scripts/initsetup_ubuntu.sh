#run the following command to install prerequisite to build CompressonatorCLI for unix then only run buildCLI_unix.sh
# openexr v2.2 
# If you want to build from source use this 
#   LD_LIBRARY_PATH=/usr/local/lib
#   export LD_LIBRARY_PATH
#   wget http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.2.0.tar.gz
#   (tar xvfz ilmbase-2.2.0.tar.gz && cd ilmbase-2.2.0 && sudo cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ilmbase22build/ . && sudo make -j 4 && sudo make install)
#   wget http://download.savannah.nongnu.org/releases/openexr/openexr-2.2.0.tar.gz
#   (tar xvfz openexr-2.2.0.tar.gz && cd openexr-2.2.0 && sudo cmake -DILMBASE_PACKAGE_PREFIX=/usr/local/ilmbase22build/  -DCMAKE_INSTALL_PREFIX=/usr/local/openexr22build/ . && sudo make -j 4 && sudo make install)

version=`lsb_release --release | cut -f2`

if [ $version = "18.04" ];then 
    echo "You have Ubuntu 18.04"
    sudo apt-get install cmake
    sudo apt-get install libglew-dev libegl1-mesa-dev
    sudo apt-get install qtdeclarative5-dev
    #openexr v2.2. is needed else have to build from source
    sudo apt-get install libopenexr-dev
    sudo apt-get install libilmbase-dev
    sudo apt-get install libopencv-dev
else
    echo "Version other than 18.04 detected. This script only support 18.04. You might need to install the prerequisite packages with the specific version as listed in initsetup_unix.sh above manually."
fi
