#run the following command to install prerequisite to build CompressonatorCLI for mac (tested only on macOS Sierra v10.12.4)
#assuming you already have homebrew installed
brew update
brew install cmake

#need >gcc-6 and >g++-6 for the build
brew install gcc
export CC=/usr/local/bin/gcc-7
export CXX=/usr/local/bin/g++-7

brew install glew
echo 'export PATH="$(brew --prefix glew)/bin:$PATH"' >> ~/.bashrc

brew install qt@5.5
echo 'export PATH="$(brew --prefix qt@5.5)/bin:$PATH"' >> ~/.bashrc

#openexr v2.2. is needed else have to build from source
#brew install openexr
#echo 'export PATH="$(brew --prefix openexr)/bin:$PATH"' >> ~/.bashrc
#to build from source, uncomment below and remember to symlink by export path
wget http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.2.0.tar.gz
(tar xvfz ilmbase-2.2.0.tar.gz && cd ilmbase-2.2.0 && ./configure && sudo make install)	
wget http://download.savannah.nongnu.org/releases/openexr/openexr-2.2.0.tar.gz
(tar xvfz openexr-2.2.0.tar.gz && cd openexr-2.2.0 && ./configure --disable-ilmbasetest && sudo make install)

#opencv v2.4.9 is needed (uncomment if brew info opencv is 2.4.9)
#brew install opencv
#echo 'export PATH="$(brew --prefix opencv@2)/lib:$PATH"' >> ~/.bashrc

brew install boost
echo 'export PATH="$(brew --prefix boost)/bin:$PATH"' >> ~/.bashrc

source ~/.bashrc

