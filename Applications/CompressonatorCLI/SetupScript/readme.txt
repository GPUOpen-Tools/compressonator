For Ubuntu build (****only v16.04LTS and v18.04LTS build tested, WSL is tested as well):

1. run initsetup_ubuntu.sh to install the required packages:
   - cmake at least 2.8.12
   - gcc-6 and g++-6 (v6.3.0) 
   - glew and opengl (libglew-dev libegl1-mesa-dev)
   - qt5 at least 5.5.1
   - openexr v2.2
   - opencv v2.49
   - Boost at least 1.59 (filesystem and system lib)
 
2. run cmake script (commands as shown below) in the Compressonator folder to build (or rebuild) all the dependencies libraries with CompressonatorCLI executable generated in the Compressonator/Applications/CompressonatorCLI folder :
cd Compressonator
cmake -G Unix\ Makefiles .
make

--------------------------------------------------------------------------------------------------------------------------------------------------------

For Mac build (only macOS Mojave v10.14.3 is tested):

1. run initsetup_mac.sh to install the required packages:
   - cmake at least 2.8.12
   - gcc-9 and g++-9 (at least > v6) 
   - glew and opengl (libglew-dev libegl1-mesa-dev)
   - qt5 at least 5.5.1
   - openexr v2.2
   - opencv
   - Boost at least 1.59 (filesystem and system lib)
 
2. run cmake script (commands as shown below) in the Compressonator folder to build (or rebuild) all the dependencies libraries with CompressonatorCLI-bin executable generated in the Compressonator/Applications/CompressonatorCLI folder :
cd Compressonator
cmake -G Unix\ Makefiles .
make

--------------------------------------------------------------------------------------------------------------------------------------------------------

For Window build:

Note: You are recommended to use the VS2015.sln solution provided under Compressonator/Applications/CompressonatorCLI/VS2015/ folder for window build.
If you want to build with cmake, make sure install the MSVC that you would like to use with cmake and continue with:

1. run the following commands with the cmake script then it will genarate and build/rebuild all the dependencies and CLI application:
cd Compressonator
cmake -G "Visual Studio 15 2017 Win64"
cmake --build . --target ALL_BUILD --config Release

--------------------------------------------------------------------------------------------------------------------------------------------------------

Known issues:
- You may encounter libdc1394 failed to initialized in Unix system. This is a library for controlling camera hardware and is linked in from opencv. Since we are not using any camera hardware for the app. Please ignore this error (it is a warning generated from opencv lib)