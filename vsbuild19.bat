set QT5_DIR=C:\Qt\Qt5.12.6\5.12.6\msvc2017_64
set VULKAN_SDK=C:\VulkanSDK\1.2.141.2

set CurrDir=%CD%
for %%* in (.) do set CurrDirName=%%~nx*
IF EXIST %CurrDir%\build (rmdir build /s /q)
mkdir build 

REM --------------------------------------------------------
REM Get Common folder content: works only for Git repo
REM --------------------------------------------------------
python %CurrDir%\scripts\fetch_dependencies.py

cd build
cmake -G "Visual Studio 16 2019" ..\..\%CurrDirName%
cd %CurrDir%


