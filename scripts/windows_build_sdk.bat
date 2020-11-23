REM compressonator build and Test
REM
REM Usage:
REM   cd <workspace>\compressonator
REM   call Scripts\windows_build_sdk.bat <workspace>

IF NOT [%WORKSPACE%] == [] set ROOTDIR=%WORKSPACE%
IF NOT [%1] == [] set ROOTDIR=%1

set WORKDIR=%ROOTDIR%\
set BUILDTMP=%ROOTDIR%\tmp

set CMAKE_PATH=cmake
set SCRIPT_DIR=%~dp0
set CURRENT_DIR=%CD%

REM Create Tmp directory for intermediate files
IF NOT EXIST "%buildTMP%" mkdir "%buildTMP%"

set _MSPDBSRV_ENDPOINT_=71078A5298C742C7B7CBF9ED261E442B

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

REM ######################################
REM compressonator - VS 2017 Release
REM ######################################

msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build Release Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build Release x64 FAILED
    cd %WORKDIR%
    exit 1
)

msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_md Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator uild release_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM compressonator - VS 2017 Debug
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_md win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM cmp_Core - VS 2017 Release
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core compressonator build Release Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build Release x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_md Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM cmp_Core - VS 2017 Debug
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build debug win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build debug x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build debug_md win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build debug_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_core build debug_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_core build debug_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_core build debug_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_core build debug_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM cmp_framework - VS2017 Release
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_md Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM cmp_framework - VS 2017 Debug
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_MD Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_MD x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_MD_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_MD_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)


REM Qt dir only required for Framework Plugins build
REM set QTDIR=C:\Qt\Qt5.12.6\5.12.6\msvc2017_64
REM 
REM REM Vulkan SDK only required for Framework Plugin build
REM set VULKAN_SDK=C:\VulkanSDK\1.2.141.2
REM 
REM ##################################################################
REM cmp_frameworkPlugins - VS 2017 Release  Reserved for V4.0
REM ##################################################################
REM msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_frameworkPlugins.sln
REM if not %ERRORLEVEL%==0 (
REM     echo on
REM     echo cmp_frameworkPlugins build release_md_dll x64 FAILED
REM     cd %WORKDIR%
REM     exit 1
REM )
REM 
REM msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\VS2017\cmp_frameworkPlugins.sln
REM if not %ERRORLEVEL%==0 (
REM     echo on
REM     echo cmp_frameworkPlugins build Debug_MD_dll x64 FAILED
REM     cd %WORKDIR%
REM     exit 1
REM )

REM #################
REM CLEAN TMP FOLDER
REM #################
rmdir "%buildTMP%" /s /q

cd %WORKDIR%
