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
rmdir /s  /q build
rmdir /s  /q lib

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
