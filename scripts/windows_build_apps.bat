REM Compressonator build CLI and GUI applications
REM
REM Usage:
REM   cd <workspace>\Compressonator
REM   call scripts\windows_build_apps.bat <workspace>

IF NOT [%WORKSPACE%] == [] set ROOTDIR=%WORKSPACE%
IF NOT [%1] == [] set ROOTDIR=%1

set WORKDIR=%ROOTDIR%\
set BUILDTMP=%ROOTDIR%\tmp

set CMAKE_PATH=cmake
set SCRIPT_DIR=%~dp0
set CURRENT_DIR=%CD%


REM Create Tmp directory for intermediate files
IF NOT EXIST "%buildTMP%" mkdir "%buildTMP%"

REM PATH setup
REM set Path=%PATH%C:\Python36;C:\Python36\Scripts;C:\Python36\Tools\Scripts;C:\Program Files\Python36\Scripts\;C:\Program Files\Python36\;

REM Qt Dir
set QTDIR=C:\Qt\Qt5.12.6\5.12.6\msvc2017_64
set QT5_DIR=C:\Qt\Qt5.12.6\5.12.6\msvc2017_64

REM Vulkan SDK Dir
set VULKAN_SDK=C:\VulkanSDK\1.2.141.2

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
python %SCRIPT_DIR%\fetch_dependencies.py

REM -----------------------------------
REM Set build version
REM -----------------------------------
IF [%BUILD_NUMBER%] == [] set BUILD_NUMBER=0

set FILEVERSION="v%MAJOR%.%MINOR%.%BUILD_NUMBER%"
python %SCRIPT_DIR%\update_version.py %MAJOR% %MINOR% %BUILD_NUMBER%

REM ######################################
REM  Set Enviornment for VS150 (VS2017)
REM ######################################
if EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Enterprise" goto :Enterprise
if EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional" goto :Professional
if EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\BuildTools" goto :Docker

echo on
echo VS2017 is not installed on this machine
cd %WORKDIR%
exit 1

:Professional
cd 
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"
goto :vscmdset

:Docker
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\BuildTools\Common7\Tools\VsDevCmd.bat"
goto :vscmdset

:Enterprise
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsDevCmd.bat"

:vscmdset

REM #####################################################################################
REM  Run cmake to generate VS2017 compressonator build all sln
REM #####################################################################################
set CurrDir=%CD%
for %%* in (.) do set CurrDirName=%%~nx*
IF EXIST %CurrDir%\build (rmdir build /s /q)
mkdir build 

cd build
cmake -G "Visual Studio 15 2017 Win64" ..\..\%CurrDirName%
cd %CurrDir%

REM #####################################################################################
REM  Compressonator : This will build all apps enabled in root folder CMakeList.txt
REM #####################################################################################
msbuild /m:4 /t:build /p:Configuration=release /p:Platform=x64   "%SCRIPT_DIR%/..\build\compressonator.sln"
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
