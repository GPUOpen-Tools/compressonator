#run the following command to install prerequisite to build CompressonatorCLI binary for mac (tested only on macOS Mojave v10.14.3)
#assuming you already have homebrew installed
brew update
brew install cmake

#need >gcc-6 and >g++-6 for the build
brew install gcc
export CC=/usr/local/bin/gcc-9
export CXX=/usr/local/bin/g++-9

brew install mesa
echo 'export PATH="$(brew --prefix glew)/bin:$PATH"' >> ~/.bashrc

brew install glew
echo 'export PATH="$(brew --prefix glew)/bin:$PATH"' >> ~/.bashrc

brew install qt
echo 'export PATH="$(brew --prefix qt)/bin:$PATH"' >> ~/.bashrc
export QT5DIR="$(brew --prefix qt)"
export CMAKE_MODULE_PATH=${QT5DIR}/lib/cmake:${CMAKE_MODULE_PATH}
export CMAKE_PREFIX_PATH=${QT5DIR}

#openexr v2.2. is needed else have to build from source
#brew install openexr
#echo 'export PATH="$(brew --prefix openexr)/bin:$PATH"' >> ~/.bashrc
#to build from source, uncomment below and remember to symlink by export path
wget http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.2.0.tar.gz
(tar xvfz ilmbase-2.2.0.tar.gz && cd ilmbase-2.2.0 && ./configure && sudo make install)	
wget http://download.savannah.nongnu.org/releases/openexr/openexr-2.2.0.tar.gz
(tar xvfz openexr-2.2.0.tar.gz && cd openexr-2.2.0 && ./configure --disable-ilmbasetest && sudo make install)

brew install opencv
echo 'export PATH="$(brew --prefix opencv@2)/lib:$PATH"' >> ~/.bashrc

brew install boost
echo 'export PATH="$(brew --prefix boost)/bin:$PATH"' >> ~/.bashrc

source ~/.bashrc

