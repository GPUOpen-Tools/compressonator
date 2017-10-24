set generator=Visual Studio 15 2017 Win64
set build=Release
setlocal
cd ../../_Plugins/CFilter/Make/
del CMakeCache.txt
rmdir /S /Q CMakeFiles
cmake -G "%generator%"
cmake --build . --target ALL_BUILD --config %build%
endlocal

setlocal
cd ../../_Plugins/CImage/DDS/Make/
del CMakeCache.txt
rmdir /S /Q CMakeFiles
cmake -G "%generator%"
cmake --build . --target ALL_BUILD --config %build%
endlocal

setlocal
cd ../../_Plugins/CImage/KTX/Make/
del CMakeCache.txt
rmdir /S /Q CMakeFiles
cmake -G "%generator%"
cmake --build . --target ALL_BUILD --config %build%
endlocal

setlocal
cd ../../_Plugins/CImage/ASTC/Make/
del CMakeCache.txt
rmdir /S /Q CMakeFiles
cmake -G "%generator%"
cmake --build . --target ALL_BUILD --config %build%
endlocal

setlocal
cd ../../_Plugins/CImage/EXR/Make/
del CMakeCache.txt
rmdir /S /Q CMakeFiles
cmake -G "%generator%"
cmake --build . --target ALL_BUILD --config %build%
endlocal

setlocal
cd ../../_Plugins/CImage/TGA/Make/
del CMakeCache.txt
rmdir /S /Q CMakeFiles
cmake -G "%generator%"
cmake --build . --target ALL_BUILD --config %build%
endlocal

setlocal
cd ../../_Plugins/CAnalysis/Analysis/Make/
del CMakeCache.txt
rmdir /S /Q CMakeFiles
cmake -G "%generator%"
cmake --build . --target ALL_BUILD --config %build%
endlocal

setlocal
cd ../../../Make/
del CMakeCache.txt
rmdir /S /Q CMakeFiles
cmake -G "%generator%"
cmake --build . --target ALL_BUILD --config %build%
endlocal

del CMakeCache.txt
rmdir /S /Q CMakeFiles
cmake -G "%generator%"  -DCMAKE_BUILD_TYPE=%build%
cmake --build . --target ALL_BUILD --config %build%

REM DLL copied for Release x64 configuration build. Please change accordingly
XCopy /r /d /y ".\..\..\..\..\Common\Lib\Ext\OpenCV\2.49\x64\VS2015\bin\release\opencv_core249.dll" ".\Release"
XCopy /r /d /y ".\..\..\..\..\Common\Lib\Ext\OpenCV\2.49\x64\VS2015\bin\release\opencv_imgproc249.dll" ".\Release"
XCopy /r /d /y ".\..\..\..\..\Common\Lib\Ext\OpenCV\2.49\x64\VS2015\bin\release\opencv_highgui249.dll" ".\Release"
XCopy /r /d /y ".\..\..\..\..\Common\Lib\Ext\glew\1.9.0\bin\x64\glew32.dll" ".\Release"
