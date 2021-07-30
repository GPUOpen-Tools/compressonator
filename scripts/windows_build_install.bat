REM save current directory to restore later
pushd %CD%

IF NOT [%1] == [] set ROOTDIR=%1
set WORKDIR=%ROOTDIR%\

PATH=C:\OpenJDK\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files\doxygen\bin;C:\Strawberry\c\bin;C:\Strawberry\perl\site\bin;C:\Strawberry\perl\bin;C:\Program Files\MiKTeX 2.9\miktex\bin\x64\;C:\Program Files (x86)\Graphviz2.38\bin;C:\Program Files\Microsoft DNX\Dnvm\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Program Files\Perforce;C:\Program Files\Perforce\DVCS\;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Users\adtbld\.dnx\bin;C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE;C:\Program Files (x86)\MSBuild\14.0\Bin;C:\Program Files\CMake\bin;C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin;C:\Users\adtbld\AppData\Local\Microsoft\WindowsApps;C:\Python27;C:\Python27\Scripts;C:\Python27\Tools\Scripts

set ADVANCED_INSTALLER="C:\Program Files (x86)\Caphyon\Advanced Installer 13.5\bin\x86\AdvancedInstaller.com"

echo on

cd %WORKDIR%\compressonator\Installer

echo --- PUBLIC INSTALLER: AMDCompressCore_64.aip
attrib -r AMDCompressCore_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressCore_64.aip /SetVersion 4.2.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /edit AMDCompressCore_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorCore_x64_4.2.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressCore_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPCore_x64.log 2>&1

echo --- PUBLIC INSTALLER: AMDCompressFrameWork_64.aip
attrib -r AMDCompressFrameWork_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressFrameWork_64.aip /SetVersion 4.2.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /edit AMDCompressFrameWork_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorFrameWork_x64_4.2.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressFrameWork_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPFrameWork_x64.log 2>&1

echo --- PUBLIC INSTALLER: AMDCompressSDK_64.aip
attrib -r AMDCompressSDK_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressSDK_64.aip /SetVersion 4.2.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /edit AMDCompressSDK_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorSDK_x64_4.2.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressSDK_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPSDK_x64.log 2>&1

echo --- PUBLIC INSTALLER: AMDCompressCLI_64.aip
attrib -r AMDCompressCLI_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressCLI_64.aip /SetVersion 4.2.%BUILD_NUMBER% 
%ADVANCED_INSTALLER% /edit AMDCompressCLI_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorCLI_x64_4.2.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressCLI_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPCLI_x64.log 2>&1

echo --- PUBLIC INSTALLER: AMDCompressGUI_64.aip
attrib -r AMDCompressGUI_64.aip
%ADVANCED_INSTALLER% /edit AMDCompressGUI_64.aip /SetVersion 4.2.%BUILD_NUMBER% 
%ADVANCED_INSTALLER% /edit AMDCompressGUI_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorGUI_x64_4.2.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build AMDCompressGUI_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPGUI_x64.log 2>&1

echo --- INTERNAL INSTALLER: AMDCompressTest_64.aip
attrib -r amdcompresstest_64.aip
%ADVANCED_INSTALLER% /edit amdcompresstest_64.aip /SetVersion 4.2.%BUILD_NUMBER% 
%ADVANCED_INSTALLER% /edit amdcompresstest_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorTest_x64_4.2.%BUILD_NUMBER%
%ADVANCED_INSTALLER% /build amdcompresstest_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPTest_x64.log 2>&1

rem echo --- INTERNAL INSTALLER: AMDCompressUpscale_64.aip
rem attrib -r amdcompressupscale_64.aip
rem %ADVANCED_INSTALLER% /edit amdcompressupscale_64.aip /SetVersion 4.2.%BUILD_NUMBER% 
rem %ADVANCED_INSTALLER% /edit amdcompressupscale_64.aip /SetProperty  SETUP_FILE_NAME=CompressonatorUpscale_x64_4.2.%BUILD_NUMBER%
rem %ADVANCED_INSTALLER% /build amdcompressupscale_64.aip -buildslist DefaultBuild > %WORKSPACE%\Setup_BuildCMPUpscale_x64.log 2>&1


cd %WORKDIR%
