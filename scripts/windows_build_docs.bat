REM --------------------------------------------------
REM Compressonator build html user documents
REM
REM Usage:
REM   cd <workspace>\compressonator
REM   call scripts\windows_build_docs.bat <workspace>
REM ---------------------------------------------------

REM save current directory to restore later

echo on

IF NOT [%1] == [] set ROOTDIR=%1
set WORKDIR=%ROOTDIR%\

REM -----------------------------------
REM PATH setup
REM -----------------------------------
set Path=C:\Python36;C:\Python36\Scripts;C:\Python36\Tools\Scripts;"C:\Program Files\Python36\Scripts\";"C:\Program Files\Python36\";C:\VC_Redist\redist\Debug_NonRedist\x64\Microsoft.VC120.DebugCRT;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;"C:\Program Files\Microsoft SQL Server\130\Tools\Binn\";"C:\Program Files\CMake\bin";"C:\Program Files\Git\cmd";%USERPROFILE%\AppData\Local\Microsoft\WindowsApps;

REM -----------------------------------
REM build html documentation
REM -----------------------------------
cd %WORKDIR%\compressonator\docs
call make html

cd %WORKDIR%
