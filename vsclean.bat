set CurrDir=%CD%
del  /s *.sdf
del  /s *.db
del  /s *.pdb
del  /s *.idb
del  /s *.ipch
del  /s *.user
del  /s *.opendb
del  /s *.orig
del  /s CMakeCache.txt
del  /s cmake_install.cmake
del  /s *.a
del  /s *.o
del  /s applications/Makefile
del  /s cmp_compressonatorlib/Makefile
del  /s cmp_core/Makefile
del  /s cmp_framework/Makefile
del  Makefile
rmdir /s  /q bin
rmdir /s  /q lib

IF EXIST %CurrDir%\build (
cd build
rmdir  /s /q  applications
rmdir  /s /q  bin
rmdir  /s /q  Cauldron_Common.dir
rmdir  /s /q  Cauldron_DX12.dir
rmdir  /s /q  cmp_compressonatorlib
rmdir  /s /q  cmp_core
rmdir  /s /q  cmp_framework
rmdir  /s /q  examples
rmdir  /s /q  external
rmdir  /s /q  lib
rmdir  /s /q  x64
del  /s *.vcxproj
del  /s *.filters
del  /s *.sln
del  /s *.cpp
del  /s *.vs
cd %CurrDir
)

cd cmp_core/shaders/
rmdir /s  /q compiled
cd %CurrDir%

FOR /d /r . %%d IN (CMakeFiles) DO @IF EXIST "%%d" rd /s /q "%%d"
FOR /d /r . %%d IN (*_autogen) DO @IF EXIST "%%d" rd /s /q "%%d"

REM Extern KTX folder cleanup
cd ../common/lib/ext/ktx
rmdir  /s /q  bin
rmdir  /s /q  build
rmdir  /s /q  src
rmdir  /s /q  tmp
cd %CurrDir%
