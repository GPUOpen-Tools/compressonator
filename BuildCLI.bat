@echo off
::-----------------------------------------------------------------
:: Build file for Compressonator CLI - May 4 2016
::
::
:: Syntax for this batch file
::
::
:: Revision History
::-----------------------------------------------------------------

:: Set this to 1 if you want VS2015 builds else leave it equal to blank
set BUILD2015=1

echo ------------------------------------------------------------------------------
echo Save Start folder at Perforce ROOT or subfolder AMD_Compress
echo ------------------------------------------------------------------------------
set BatchDir=%CD%
echo --1
IF EXIST %BatchDir%\Compressonator     (set COMPRESSONATOR_ROOT=%CD%\Compressonator)
IF EXIST %BatchDir%\Common             (set COMMON_ROOT=%CD%\Common)
IF [%COMPRESSONATOR_ROOT%]==[]         (set COMPRESSONATOR_ROOT=%CD%)
echo --2

echo -------------------------------------------------------------------------------
echo  Output Log File
echo -------------------------------------------------------------------------------
set OUTPUT_LOG=%BatchDir%\output.log

echo -----------------------------------------------------------------------------
echo Check for MSBUILD existence based on Version 14.0 (MSBUILD SDK installs)
echo -----------------------------------------------------------------------------
set Building="Check Missing MSBUILD v14 -----------------------"
echo --1
echo %Building%  >> %OUTPUT_LOG%
echo --2
reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\14.0" /v MSBuildToolsPath > nul 2>&1
if ERRORLEVEL 1 goto MSBuildTest2
echo --3
for /f "skip=2 tokens=2,*" %%A in ('reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\14.0" /v MSBuildToolsPath') do SET MSBUILDDIR=%%B
IF NOT EXIST "%MSBUILDDIR%msbuild.exe" goto MSBuildTest2
echo --4
set PATH=%MSBUILDDIR%;%PATH%
echo --5
echo -----------------------------------------------------------------------------
echo Get MSBUILD prop path
echo -----------------------------------------------------------------------------
reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\14.0" /v MSBuildToolsRoot > nul 2>&1
if ERRORLEVEL 1 goto MSBuildTest2
echo --1
for /f "skip=2 tokens=2,*" %%A in ('reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\14.0" /v MSBuildToolsRoot') do SET MSBUILDROOT=%%B
echo --2
set PATH=%MSBUILDROOT%\Microsoft.Cpp\v4.0\;%PATH%
echo -----------------------------------------------------------------------------
echo Set MSBUILD prop path for VS2015 
echo -----------------------------------------------------------------------------
set PATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE;C:\Program Files (x86)\MSBuild\14.0\Bin;C:\Program Files\CMake\bin;C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin;%MSBUILDROOT%\Microsoft.Cpp\v4.0\V140\;%PATH%
echo --1
PATH >> %OUTPUT_LOG%
echo --2
goto RunBatch:
echo --------------------------------------------------------------------
echo Check for MSBUILD existence based on Version 4.0 (.Net installs)
echo --------------------------------------------------------------------
:MSBuildTest2
echo --1
set Building="Check Missing MSBUILD v4 -----------------------"
echo --2
echo %Building%  >> %OUTPUT_LOG%
echo --3
reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\4.0" /v MSBuildToolsPath > nul 2>&1
if ERRORLEVEL 1 goto Error
echo --4
for /f "skip=2 tokens=2,*" %%A in ('reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\4.0" /v MSBuildToolsPath') do SET MSBUILDDIR=%%B
echo --5
IF NOT EXIST "%MSBUILDDIR%msbuild.exe" goto Error
echo --6
set PATH=%MSBUILDDIR%;%PATH%
::--------------------------------------------------------------------
:: THE FOLLOWING ARE NOT CHECKED FOR based on older (.Net installs)
:: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\MSBuild\ToolsVersions\2.0
:: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\MSBuild\ToolsVersions\3.5
::--------------------------------------------------------------------
echo -------------------------------------------------------
echo Set ENVIORONMENT variables and build type
echo -------------------------------------------------------
:RunBatch
echo --1
set ORIG_PATH=%PATH%
echo --2
set Building="Set Enviornment Variables - Please check -----------------------"
echo --3
echo %Building%  >> %OUTPUT_LOG%
::
echo ------------------------------------------------------------
echo VS2015 Build CompressonatorCLI Exe
echo ------------------------------------------------------------
cd %COMPRESSONATOR_ROOT%\Applications\CompressonatorCLI\VS2015
::
set Building="STEP  a2:  VS2015 Build CompressonatorCLI x64:Release -----------------------"
echo --1
echo %Building%  >> %OUTPUT_LOG%
echo --2
msbuild /m:6 /t:rebuild /p:Configuration=Release_MD /p:Platform=x64   "VS2015.sln" >> %OUTPUT_LOG%
IF %ERRORLEVEL% GTR 0 goto Error
::
Goto Done
:Error
::
echo -----------------------------------------------------------    >> %OUTPUT_LOG%
echo ***** !!Build Failed!! for %Building%                          >> %OUTPUT_LOG%
echo ***** MSBuild Exit Error = %errorlevel%                        >> %OUTPUT_LOG%
echo ------------------------------------------------------------   >> %OUTPUT_LOG%
::
cd %BatchDir%
echo "BUILD FAILED"
exit 1
:Done
:: ---------------------------------------------------
:: Build DONE return to original start folder  
:: ---------------------------------------------------
echo "BUILD PASSED"  >> %OUTPUT_LOG%
cd %BatchDir%
echo "BUILD PASSED"
:Finish