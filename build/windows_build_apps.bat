REM Compressonator build CLI and GUI applications
REM
REM Usage:
REM   cd <workspace>\Compressonator
REM   call build\windows_build_apps.bat <workspace>
REM
REM Optionally, you can set the CMAKE_ARGS environment variable to add extra options
REM     to the CMake generation step of the build

IF NOT [%WORKSPACE%] == [] set ROOTDIR=%WORKSPACE%
IF NOT [%1] == [] set ROOTDIR=%1

set WORKDIR=%ROOTDIR%\
set BUILDTMP=%ROOTDIR%\tmp

set CMAKE_PATH=cmake
set BUILD_SCRIPT_DIR=%~dp0
set SCRIPT_DIR=%BUILD_SCRIPT_DIR%\..\build
set CURRENT_DIR=%CD%


REM Create Tmp directory for intermediate files
IF NOT EXIST "%buildTMP%" mkdir "%buildTMP%"

REM PATH setup
REM set Path=%PATH%C:\Python36;C:\Python36\Scripts;C:\Python36\Tools\Scripts;C:\Program Files\Python36\Scripts\;C:\Program Files\Python36\;

REM Setting dependency locations for Compressonator
set QT_DIR=C:\Qt\Qt5.12.6\5.12.6\msvc2017_64
set VULKAN_DIR=C:\VulkanSDK\1.2.141.2
set OPENCV_DIR=C:\opencv\

REM "%WORKDIR%\compressonator\cmp_compressonatorLib\version.h" needs to be writable
REM These lines are parsed to extract version information. The build number
REM is added from the environment to give a complete build number which is
REM then used to update a number of files in both compressonator folder

set MAJOR=
for /f "delims=" %%a in ('python %SCRIPT_DIR%\get_version.py --major') do @set MAJOR=%%a
set MINOR=
for /f "delims=" %%a in ('python %SCRIPT_DIR%\get_version.py --minor') do @set MINOR=%%a


REM --------------------------------------------------------
REM Get Common folder content: works only for Git repo
REM --------------------------------------------------------
python %BUILD_SCRIPT_DIR%\fetch_dependencies.py

REM -----------------------------------
REM Set build version
REM -----------------------------------
IF [%BUILD_NUMBER%] == [] set BUILD_NUMBER=0

set FILEVERSION="v%MAJOR%.%MINOR%.%BUILD_NUMBER%"
python %SCRIPT_DIR%\update_version.py %MAJOR% %MINOR% %BUILD_NUMBER%

REM ################################
REM  Set Enviornment for (VS2019)
REM ###############################
if EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise" goto :Enterprise
if EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional" goto :Professional
if EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools" goto :Docker

echo on
echo VS2019 is not installed on this machine
cd %WORKDIR%
exit 1

:Professional
cd 
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
goto :vscmdset

:Docker
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\Common7\Tools\VsDevCmd.bat"
goto :vscmdset

:Enterprise
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\Common7\Tools\VsDevCmd.bat"

:vscmdset

REM #####################################################################################
REM  Run cmake to generate VS2019 compressonator build all sln
REM #####################################################################################
set CurrDir=%CD%
for %%* in (.) do set CurrDirName=%%~nx*
IF EXIST %CurrDir%\build\bin (rmdir build\bin /s /q)
mkdir build\bin

cd build\bin
cmake %CMAKE_ARGS% -DCMP_VERSION_MAJOR=%MAJOR% -DCMP_VERSION_MINOR=%MINOR% -DCMP_VERSION_BUILD_NUMBER=%BUILD_NUMBER% -G "Visual Studio 16 2019" ..\..\..\%CurrDirName%
cd %CurrDir%

REM #####################################################################################
REM  Compressonator : This will build all apps enabled in root folder CMakeList.txt
REM #####################################################################################
msbuild /m:4 /t:build /p:Configuration=release /p:Platform=x64   "%BUILD_SCRIPT_DIR%\bin\compressonator.sln"
if not %ERRORLEVEL%==0 (
    echo build of compressonator release apps x64 FAILED!
    cd %WORKDIR%
    exit /b 1
)

REM #################
REM CLEAN TMP FOLDER
REM #################
cd ..
rmdir "%BUILDTMP%" /s /q

cd %WORKDIR%
