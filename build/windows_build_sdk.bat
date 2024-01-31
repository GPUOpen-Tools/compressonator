REM compressonator build and Test
REM
REM Usage:
REM   cd <workspace>\compressonator
REM   call Scripts\windows_build_sdk.bat <workspace>

IF NOT [%WORKSPACE%] == [] set ROOTDIR=%WORKSPACE%
IF NOT [%1] == [] set ROOTDIR=%1

set WORKDIR=%ROOTDIR%\
set BUILDTMP=%ROOTDIR%\tmp

REM Create Tmp directory for intermediate files
IF NOT EXIST "%buildTMP%" mkdir "%buildTMP%"

set _MSPDBSRV_ENDPOINT_=71078A5298C742C7B7CBF9ED261E442B

REM ######################################
REM  Set Enviornment for Visual Studio
REM ######################################
if EXIST "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise" goto :Enterprise
if EXIST "%ProgramFiles%\Microsoft Visual Studio\2022\Professional" goto :Professional
if EXIST "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" goto :Docker
if EXIST "%ProgramFiles%\Microsoft Visual Studio\2022\Community" goto :Community

echo on
echo VS2022 is not installed on this machine
cd %WORKDIR%
exit 1

:Professional
cd 
call "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
goto :vscmdset

:Docker
call "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
goto :vscmdset

:Enterprise
call "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
goto :vscmdset

:Community
call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

:vscmdset

REM ######################################
REM compressonator - VS 2022 Release
REM ######################################

msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build Release Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build Release x64 FAILED
    cd %WORKDIR%
    exit 1
)

msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_md Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build release_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM compressonator - VS 2022 Debug
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_md win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_compressonatorLib.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo compressonator build debug_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM cmp_Core - VS 2022 Release
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core compressonator build Release Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build Release x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_md Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build release_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM cmp_Core - VS 2022 Debug
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build debug win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build debug x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build debug_md win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_Core build debug_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_core build debug_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_core build debug_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_core build debug_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_Core.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_core build debug_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM cmp_framework - VS 2022 Release
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_md Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_md x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_md_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Release_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build release_md_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM ######################################
REM cmp_framework - VS 2022 Debug
REM ######################################
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_MD Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_MD x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=Win32 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_MD_dll Win32 FAILED
    cd %WORKDIR%
    exit 1
)
msbuild /m:4 /t:build /p:Configuration=Debug_MD_DLL /p:Platform=x64 %WORKDIR%\compressonator\build_sdk\cmp_framework.sln
if not %ERRORLEVEL%==0 (
    echo on
    echo cmp_framework build Debug_MD_dll x64 FAILED
    cd %WORKDIR%
    exit 1
)

REM #################
REM CLEAN TMP FOLDER
REM #################
rmdir "%buildTMP%" /s /q

cd %WORKDIR%
