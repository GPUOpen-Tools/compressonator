REM Compressonator build CLI and GUI applications
REM
REM Usage:
REM   cd <workspace>\Compressonator
REM   call scripts\windows_build_apps.bat <workspace>

IF NOT [%1] == [] set ROOTDIR=%1
set WORKDIR=%ROOTDIR%\
set BUILDTMP=%ROOTDIR%\tmp

REM Create Tmp directory for intermediate files
IF NOT EXIST "%buildTMP%" mkdir "%buildTMP%"

REM PATH setup
set Path=C:\Python36;C:\Python36\Scripts;C:\Python36\Tools\Scripts;"C:\Program Files\Python36\Scripts\";"C:\Program Files\Python36\";C:\VC_Redist\redist\Debug_NonRedist\x64\Microsoft.VC120.DebugCRT;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;"C:\Program Files\Microsoft SQL Server\130\Tools\Binn\";"C:\Program Files\CMake\bin";"C:\Program Files\Git\cmd";%USERPROFILE%\AppData\Local\Microsoft\WindowsApps;

REM Qt Dir
set QTDIR=C:\Qt\Qt5.12.6\5.12.6\msvc2017_64

REM Vulkan SDK Dir
set VULKAN_SDK=C:\VulkanSDK\1.2.141.2

REM "%WORKDIR%\compressonator\cmp_compressonatorLib\version.h" needs to be writable
REM These lines are parsed to extract version information. The build number
REM is added from the environment to give a complete build number which is
REM then used to update a number of files in both compressonator folder

set MAJOR=
for /f "delims=" %%a in ('python %WORKDIR%\compressonator\scripts\get_version.py --major') do @set MAJOR=%%a
set MINOR=
for /f "delims=" %%a in ('python %WORKDIR%\compressonator\scripts\get_version.py --minor') do @set MINOR=%%a


REM --------------------------------------------------------
REM Get Common folder content: works only for Git repo
REM --------------------------------------------------------
python %WORKDIR%\compressonator\build\fetch_dependencies.py

REM -----------------------------------
REM Set build version
REM -----------------------------------
set FILEVERSION="v%MAJOR%.%MINOR%.%BUILD_NUMBER%"
python %WORKDIR%\compressonator\scripts\update_version.py %MAJOR% %MINOR% %BUILD_NUMBER% %MINOR%

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
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\common7\tools\VsDevCmd.bat"
goto :vscmdset

:Docker
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\BuildTools\common7\tools\VsDevCmd.bat"
goto :vscmdset

:Enterprise
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\enterprise\common7\tools\VsDevCmd.bat"

:vscmdset

REM #####################################################################################
REM  CompressonatorCLI COMMAND LINE VS2017 ToDo Change Folder names to be more generic
REM #####################################################################################
msbuild /m:4 /t:Build /p:Configuration=Release_MD /p:Platform=x64   "%WORKDIR%\compressonator\applications\compressonatorCLI\VS2017\compressonatorCLI.sln"
if not %ERRORLEVEL%==0 (
    echo build of compressonatorCLI release_md x64 FAILED!
    cd %WORKDIR%
    exit 1
)

REM REM ############################
REM  Compressonator GUI VS2017
REM ############################
msbuild /m:4 /t:Build /p:Configuration=Release_MD /p:Platform=x64   "%WORKDIR%\compressonator\applications\compressonatorGUI\VS2017\compressonatorGUI.sln"
if not %ERRORLEVEL%==0 (
    echo build of compressonatorGUI release_md x64 FAILED!
    cd %WORKDIR%
    exit 1
)

REM #################
REM CLEAN TMP FOLDER
REM #################
cd ..
rmdir "%BUILDTMP%" /s /q

cd %WORKDIR%
