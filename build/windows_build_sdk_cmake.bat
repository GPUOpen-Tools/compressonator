REM Compressonator build SDK

REM Setting dependency locations for Compressonator
set QT_DIR=C:\Qt\Qt5.12.6\5.12.6\msvc2017_64
set VULKAN_DIR=C:\VulkanSDK\1.2.141.2
set OPENCV_DIR=C:\opencv\
set CurrDir=%CD%

REM --------------------------------------------------------
REM Get Common folder content: works only for Git repo
REM --------------------------------------------------------
python %CurrDir%\fetch_dependencies.py

REM ################################
REM  Set Enviornment for (VS2022)
REM ###############################
if EXIST "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise" goto :Enterprise
if EXIST "%ProgramFiles%\Microsoft Visual Studio\2022\Professional" goto :Professional
if EXIST "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" goto :Docker
if EXIST "%ProgramFiles%\Microsoft Visual Studio\2022\Community" goto :Community

echo on
echo VS2022 is not installed on this machine
cd %CurrDir%
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
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

:vscmdset

REM #####################################################################################
REM  Run cmake to generate VS2022 compressonator build all sln
REM #####################################################################################

IF EXIST %CurrDir%\sdk\bin\64 (rmdir sdk\bin\64 /s /q)
mkdir sdk\bin\64

IF EXIST %CurrDir%\sdk\bin\32 (rmdir sdk\bin\32 /s /q)
mkdir sdk\bin\32

cd sdk\bin\64
cmake %CMAKE_ARGS% -G "Visual Studio 17 2022" -A x64 ..\..
cd %CurrDir%

cd sdk\bin\32
cmake %CMAKE_ARGS% -G "Visual Studio 17 2022" -A Win32 ..\..
cd %CurrDir%

REM #####################################################################################
REM  Compressonator : This will build all apps enabled in sdk folder CMakeList.txt
REM #####################################################################################

REM Release 64 bit
msbuild /m:4 /t:build /p:Configuration=release /p:Platform=x64 ".\sdk\bin\64\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator release apps x64 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=release_md /p:Platform=x64 ".\sdk\bin\64\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator release_md apps x64 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=release_dll /p:Platform=x64 ".\sdk\bin\64\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator release_dll apps x64 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=release_md_dll /p:Platform=x64 ".\sdk\bin\64\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator release_md_dll apps x64 FAILED!
    cd %WORKDIR%
    exit /b 1
)

REM Debug 64 bit
msbuild /m:4 /t:build /p:Configuration=debug /p:Platform=x64 ".\sdk\bin\64\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator debug apps x64 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=debug_md /p:Platform=x64 ".\sdk\bin\64\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator debug_md apps x64 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=debug_dll /p:Platform=x64 ".\sdk\bin\64\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator debug_dll apps x64 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=debug_md_dll /p:Platform=x64 ".\sdk\bin\64\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator debug_md_dll apps x64 FAILED!
    cd %WORKDIR%
    exit /b 1
)

REM Release 32 bit
msbuild /m:4 /t:build /p:Configuration=release /p:Platform=Win32 ".\sdk\bin\32\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator release apps Win32 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=release_md /p:Platform=Win32 ".\sdk\bin\32\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator release_md apps Win32 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=release_dll /p:Platform=Win32 ".\sdk\bin\32\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator release_dll apps Win32 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=release_md_dll /p:Platform=Win32 ".\sdk\bin\32\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator release_md_dll apps Win32 FAILED!
    cd %WORKDIR%
    exit /b 1
)

REM Debug 32 bit
msbuild /m:4 /t:build /p:Configuration=debug /p:Platform=Win32 ".\sdk\bin\32\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator debug apps Win32 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=debug_md /p:Platform=Win32 ".\sdk\bin\32\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator debug_md apps Win32 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=debug_dll /p:Platform=Win32 ".\sdk\bin\32\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator debug_dll apps Win32 FAILED!
    cd %WORKDIR%
    exit /b 1
)

msbuild /m:4 /t:build /p:Configuration=debug_md_dll /p:Platform=Win32 ".\sdk\bin\32\compressonatorSDK.sln
if not %ERRORLEVEL%==0 (
    echo build of compressonator debug_md_dll apps Win32 FAILED!
    cd %WORKDIR%
    exit /b 1
)


