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
cmake -G "Visual Studio 15 2017 Win64" ..\..\%CurrDirName%
REM cmake -DOPTION_ENABLE_ALL_APPS=OFF -DOPTION_BUILD_APPS_CMP_CLI=ON -G "Visual Studio 15 2017 Win64"  ..\..\%CurrDirName%
cd %CurrDir%


