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

IF EXIST %CurrDir%\build\bin (
    rmdir /s /q %CurrDir%\build\bin
    cd %CurrDir
)

cd cmp_core/shaders/
rmdir /s  /q compiled
cd %CurrDir%

FOR /d /r . %%d IN (CMakeFiles) DO @IF EXIST "%%d" rd /s /q "%%d"
FOR /d /r . %%d IN (*_autogen) DO @IF EXIST "%%d" rd /s /q "%%d"

REM Extern KTX folder cleanup
IF EXIST %CurrDir%\..\common\lib\ext\ktx (
    cd ../common/lib/ext/ktx
    rmdir  /s /q  bin
    rmdir  /s /q  build
    rmdir  /s /q  src
    rmdir  /s /q  tmp
    cd %CurrDir%
)

REM Extern Brotli-G folder cleanup
IF EXIST %CurrDir%\..\common\lib\ext\brotlig (
    cd ../common/lib/ext/brotlig
    rmdir  /s /q  build
    cd %CurrDir%
)

REM Extern GLFW folder cleanup
IF EXIST %CurrDir%\..\common\lib\ext\glfw (
    cd ../common/lib/ext/glfw
    rmdir  /s /q  build
    cd %CurrDir%
)