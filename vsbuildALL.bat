set QT_DIR=C:\Qt\Qt5.12.6\5.12.6\msvc2017_64
set VULKAN_DIR=C:\VulkanSDK\1.2.141.2
set OPENCV_DIR=C:\opencv\

set CurrDir=%CD%
for %%* in (.) do set CurrDirName=%%~nx*

REM --------------------------------------------------------
REM Get Common folder content: works only for Git repo
REM --------------------------------------------------------
python %CurrDir%\build\fetch_dependencies.py

mkdir build\bin

cd build\bin
cmake -G "Visual Studio 16 2019" ..\..\..\%CurrDirName%\
cd %CurrDir%


