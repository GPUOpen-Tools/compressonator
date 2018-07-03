#run the following command to install prerequisite to build CompressonatorCLI for unix then only run buildCLI_unix.sh
version=`lsb_release --release | cut -f2`

if [ $version = "18.04" ];then
	echo "You have Ubuntu 18.04"
	sudo apt-get install cmake
	sudo apt-get install libglew-dev libegl1-mesa-dev
	sudo apt-get install qtdeclarative5-dev
	#openexr v2.2. is needed else have to build from source
	wget http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.2.0.tar.gz
	(tar xvfz ilmbase-2.2.0.tar.gz && cd ilmbase-2.2.0 && sudo cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ilmbase22build/ . && sudo make -j 4 && sudo make install)
	wget http://download.savannah.nongnu.org/releases/openexr/openexr-2.2.0.tar.gz
	(tar xvfz openexr-2.2.0.tar.gz && cd openexr-2.2.0 && sudo cmake -DILMBASE_PACKAGE_PREFIX=/usr/local/ilmbase22build/  -DCMAKE_INSTALL_PREFIX=/usr/local/openexr22build/ . && sudo make -j 4 && sudo make install)
	
	sudo apt-get install libopencv-dev
	sudo apt-get install libboost-filesystem-dev
	sudo apt-get install libboost-system-dev
elif [ $version = "16.04" ];then
	echo "You have Ubuntu 16.04"
	sudo apt-get install cmake
	#need gcc-6 and g++-6 for the build
	sudo add-apt-repository ppa:ubuntu-toolchain-r/test
	sudo apt-get update
	sudo apt-get install gcc-6 g++-6
	export CC=/usr/bin/gcc-6
	export CXX=/usr/bin/g++-6
	sudo apt-get install libglew-dev libegl1-mesa-dev
	sudo apt-get install qtdeclarative5-dev
	#openexr v2.2. is needed else have to build from source
	wget http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.2.0.tar.gz
	(tar xvfz ilmbase-2.2.0.tar.gz && cd ilmbase-2.2.0 && sudo cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ilmbase22build/ . && sudo make -j 4 && sudo make install)
	wget http://download.savannah.nongnu.org/releases/openexr/openexr-2.2.0.tar.gz
	(tar xvfz openexr-2.2.0.tar.gz && cd openexr-2.2.0 && sudo cmake -DILMBASE_PACKAGE_PREFIX=/usr/local/ilmbase22build/  -DCMAKE_INSTALL_PREFIX=/usr/local/openexr22build/ . && sudo make -j 4 && sudo make install)
	
	sudo apt-get install libopencv-dev
	sudo apt-get install libboost-filesystem-dev
	sudo apt-get install libboost-system-dev
elif [ $version = "14.04" ];then
	echo "You have Ubuntu 14.04"
	sudo apt-get install cmake
	#need gcc-6 and g++-6 for the build
	sudo add-apt-repository ppa:ubuntu-toolchain-r/test
	sudo apt-get update
	sudo apt-get install gcc-6 g++-6
	export CC=/usr/bin/gcc-6
	export CXX=/usr/bin/g++-6
	sudo apt-get install libglew-dev libegl1-mesa-dev
	#need qt>=5.4 to run the program
	sudo add-apt-repository ppa:kvirc/kvirc-qt5.5
	sudo apt-get update
	sudo apt-get install qtdeclarative5-dev

	#need to build openexr v2.2
	LD_LIBRARY_PATH=/usr/local/lib
	export LD_LIBRARY_PATH
	wget http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.2.0.tar.gz
	(tar xvfz ilmbase-2.2.0.tar.gz && cd ilmbase-2.2.0 && sudo cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ilmbase22build/ . && sudo make -j 4 && sudo make install)
	wget http://download.savannah.nongnu.org/releases/openexr/openexr-2.2.0.tar.gz
	(tar xvfz openexr-2.2.0.tar.gz && cd openexr-2.2.0 && sudo cmake -DILMBASE_PACKAGE_PREFIX=/usr/local/ilmbase22build/  -DCMAKE_INSTALL_PREFIX=/usr/local/openexr22build/ . && sudo make -j 4 && sudo make install)
	
	sudo apt-get install libopencv-dev
	sudo apt-get install libboost-filesystem-dev
	sudo apt-get install libboost-system-dev
	
else
	echo "Version other than 14.04 and 16.04 detected. This script only support 16.04 and 14.04 build. You might need to install the prerequisite packages with the specific version as listed in initsetup_unix.sh above manually."
fi


