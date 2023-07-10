REM save current directory to restore later
pushd %CD%

IF NOT [%1] == [] set ROOTDIR=%1
set WORKDIR=%ROOTDIR%\

set SCRIPT_DIR=%~dp0\..\scripts

set ADVANCED_INSTALLER="C:\Program Files (x86)\Caphyon\Advanced Installer 13.5\bin\x86\AdvancedInstaller.com"

REM "%WORKDIR%\compressonator\cmp_compressonatorLib\version.h" needs to be writable
REM These lines are parsed to extract version information. The build number
REM is added from the environment to give a complete build number which is
REM then used to update a number of files in both compressonator folder

set MAJOR=
for /f "delims=" %%a in ('python %SCRIPT_DIR%\get_version.py --major') do @set MAJOR=%%a
set MINOR=
for /f "delims=" %%a in ('python %SCRIPT_DIR%\get_version.py --minor') do @set MINOR=%%a

echo on

cd %WORKDIR%\compressonator\Installer

echo --- PUBLIC INSTALLER: AMDCompressCore_64.aip
attrib -r AMDCompressCore_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressCore_64.aip /SetVersion %MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /edit AMDCompressCore_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorCore_x64_%MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressCore_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPCore_x64.log 2>&1

if not %ERRORLEVEL%==0 (
    echo on
    echo Compressonator Core Installer failed
    cd %WORKDIR%
    exit 1
)


echo --- PUBLIC INSTALLER: AMDCompressFrameWork_64.aip
attrib -r AMDCompressFrameWork_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressFrameWork_64.aip /SetVersion %MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /edit AMDCompressFrameWork_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorFrameWork_x64_%MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressFrameWork_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPFrameWork_x64.log 2>&1

if not %ERRORLEVEL%==0 (
    echo on
    echo Compressonator Framework Installer failed
    cd %WORKDIR%
    exit 1
)

echo --- PUBLIC INSTALLER: AMDCompressSDK_64.aip
attrib -r AMDCompressSDK_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressSDK_64.aip /SetVersion %MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /edit AMDCompressSDK_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorSDK_x64_%MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressSDK_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPSDK_x64.log 2>&1

if not %ERRORLEVEL%==0 (
    echo on
    echo Compressonator SDK Installer failed
    cd %WORKDIR%
    exit 1
)

echo --- PUBLIC INSTALLER: AMDCompressCLI_64.aip
attrib -r AMDCompressCLI_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressCLI_64.aip /SetVersion %MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /edit AMDCompressCLI_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorCLI_x64_%MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressCLI_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPCLI_x64.log 2>&1

if not %ERRORLEVEL%==0 (
    echo on
    echo Compressonator CLI Installer failed
    cd %WORKDIR%
    exit 1
)

echo --- PUBLIC INSTALLER: AMDCompressGUI_64.aip
attrib -r AMDCompressGUI_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressGUI_64.aip /SetVersion %MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /edit AMDCompressGUI_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorGUI_x64_%MAJOR%.%MINOR%.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressGUI_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPGUI_x64.log 2>&1

if not %ERRORLEVEL%==0 (
    echo on
    echo Compressonator GUI Installer failed
    cd %WORKDIR%
    exit 1
)

cd %WORKDIR%
