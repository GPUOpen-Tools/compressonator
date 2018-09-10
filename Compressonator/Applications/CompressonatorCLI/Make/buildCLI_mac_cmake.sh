#please make sure all the prerequite packages are installed. You can refer to or run initsetup_mac.sh to install the required package to run this shell script
set -e
export CC=/usr/local/bin/gcc-7
export CXX=/usr/local/bin/g++-7

rm -f ../../_Plugins/CImage/DDS/Make/CMakeCache.txt 
rm -f -r ../../_Plugins/CImage/DDS/Make/CMakeFiles
rm -f ../../_Plugins/CImage/DDS/Make/cmake_install.cmake
rm -f ../../_Plugins/CImage/DDS/Make/Makefile
(cd ../../_Plugins/CImage/DDS/Make/ && cmake -G Unix\ Makefiles && make)

rm -f ../../_Plugins/CImage/ASTC/Make/CMakeCache.txt 
rm -f -r ../../_Plugins/CImage/ASTC/Make/CMakeFiles
rm -f ../../_Plugins/CImage/ASTC/Make/cmake_install.cmake
rm -f ../../_Plugins/CImage/ASTC/Make/Makefile
(cd ../../_Plugins/CImage/ASTC/Make/ && cmake -G Unix\ Makefiles && make)

rm -f ../../_Plugins/CImage/EXR/Make/CMakeCache.txt 
rm -f -r ../../_Plugins/CImage/EXR/Make/CMakeFiles
rm -f ../../_Plugins/CImage/EXR/Make/cmake_install.cmake
rm -f ../../_Plugins/CImage/EXR/Make/Makefile
(cd ../../_Plugins/CImage/EXR/Make/ && cmake -G Unix\ Makefiles && make)

rm -f ../../_Plugins/CImage/TGA/Make/CMakeCache.txt 
rm -f -r ../../_Plugins/CImage/TGA/Make/CMakeFiles
rm -f ../../_Plugins/CImage/TGA/Make/cmake_install.cmake
rm -f ../../_Plugins/CImage/TGA/Make/Makefile
(cd ../../_Plugins/CImage/TGA/Make/ && cmake -G Unix\ Makefiles && make)

rm -f ../../_Plugins/CImage/KTX/Make/CMakeCache.txt 
rm -f -r ../../_Plugins/CImage/KTX/Make/CMakeFiles
rm -f ../../_Plugins/CImage/KTX/Make/cmake_install.cmake
rm -f ../../_Plugins/CImage/KTX/Make/Makefile
(cd ../../_Plugins/CImage/KTX/Make/ && cmake -G Unix\ Makefiles && make)

if [ $? -ne 0 ]; then
    echo If you\'re receiving glcorearb.h missing error you can try downloading by running:
    echo sudo wget -P /usr/local/include/GL http://www.opengl.org/registry/api/GL/glcorearb.h
    exit
fi

rm -f ../../_Plugins/CFilter/Make/CMakeCache.txt 
rm -f -r ../../_Plugins/CFilter/Make/CMakeFiles
rm -f ../../_Plugins/CFilter/Make/cmake_install.cmake
rm -f ../../_Plugins/CFilter/Make/Makefile
(cd ../../_Plugins/CFilter/Make/ && cmake -G Unix\ Makefiles && make)

rm -f ../../_Plugins/CAnalysis/Analysis/Make/CMakeCache.txt 
rm -f -r ../../_Plugins/CAnalysis/Analysis/Make/CMakeFiles
rm -f ../../_Plugins/CAnalysis/Analysis/Make/cmake_install.cmake
rm -f ../../_Plugins/CAnalysis/Analysis/Make/Makefile
(cd ../../_Plugins/CAnalysis/Analysis/Make/ && cmake -G Unix\ Makefiles && make)

rm -f ../../../Make/CMakeCache.txt 
rm -f -r ../../../Make/CMakeFiles
rm -f ../../../Make/cmake_install.cmake
rm -f ../../../Make/Makefile
(cd ../../../Make/ && cmake -G Unix\ Makefiles && make)

#comment out the section below if you rebuild all libs from cmakelist to prevent endless cycle 
rm -f CMakeCache.txt 
rm -f -r CMakeFiles
rm -f make_install.cmake
rm -f Makefile
(cmake -G Unix\ Makefiles && make)

