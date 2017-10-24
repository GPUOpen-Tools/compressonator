****Note: For all build, Do NOT move the shell/batch scripts provided (run in the same folder (Compressonator/Applications/CompressonatorCLI/Make/))

For Ubuntu build (****only v14.04LTS and v16.04LTS build tested, WSL is tested as well):

1. run initsetup_ubuntu.sh to install the required packages:
   - cmake at least 2.8.12
   - gcc-6 and g++-6 (v6.3.0) 
   - glew and opengl (libglew-dev libegl1-mesa-dev)
   - qt5 at least 5.5.1
   - openexr v2.2
   - opencv v2.49
   - Boost at least 1.59 (filesystem and system lib)
 
2. run buildCLI_ubuntu_cmake.sh to build (or rebuild) all the dependencies libraries with CompressonatorCLI executable generated in the same folder

--------------------------------------------------------------------------------------------------------------------------------------------------------

For Mac build (only Mac Sierra is tested):

1. run initsetup_mac.sh to install the required packages:
   - cmake at least 2.8.12
   - gcc-6 and g++-6 (v6.3.0) 
   - glew and opengl (libglew-dev libegl1-mesa-dev)
   - qt5 at least 5.5.1
   - openexr v2.2
   - opencv v2.49 (opencv optional- this is required for analysis plugin but it is currently disabled in Mac due to this dependency)
   - Boost at least 1.59 (filesystem and system lib)
 
2. run buildCLI_mac_cmake.sh to build (or rebuild) all the dependencies libraries with CompressonatorCLI executable generated in the same folder

--------------------------------------------------------------------------------------------------------------------------------------------------------

For Window build (Win10 Generator VS2017 is tested):

Note: You are recommended to use the VS2015.sln solution provided under Compressonator/Applications/CompressonatorCLI/VS2015/ folder for window build.
If you want to build with cmake, make sure install the MSVC that you would like to use with cmake and continue with:

1. run the window batch file (buildCLI_win_cmake.bat) then it will build/rebuild all the dependencies and CLI app in the same folder.

--------------------------------------------------------------------------------------------------------------------------------------------------------

Known issues:
- For mac build, opencv is disabled due to older version (v2.49) is used. will update opencv in future release
- In some system, you may encounter libdc1394 failed to initialized. This is a library for controlling camera hardware and is linked in from opencv. Since we are not using any camera hardware for the app. Please ignore this error (it is a warning generated from opencv lib)