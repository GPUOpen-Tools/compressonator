@echo off
rem Copyright (c) Microsoft Corporation. All rights reserved.
rem Licensed under the MIT License.

setlocal
set error=0

set FXCOPTS=/nologo /Ges /Zi /Zpc /Qstrip_reflect /Qstrip_debug

set PCFXC="%WindowsSdkVerBinPath%x86\fxc.exe"
if exist %PCFXC% goto continue
set PCFXC="%WindowsSdkBinPath%%WindowsSDKVersion%\x86\fxc.exe"
if exist %PCFXC% goto continue
set PCFXC="%WindowsSdkDir%bin\%WindowsSDKVersion%\x86\fxc.exe"
if exist %PCFXC% goto continue

set PCFXC=fxc.exe

:continue
@if not exist compiled mkdir compiled
call :compileshader bc7_encode_kernel TryMode456CS
call :compileshader bc7_encode_kernel TryMode137CS
call :compileshader bc7_encode_kernel TryMode02CS
call :compileshader bc7_encode_kernel EncodeBlocks

call :compileshader bc6_encode_kernel TryModeG10CS
call :compileshader bc6_encode_kernel TryModeLE10CS
call :compileshader bc6_encode_kernel EncodeBlocks

echo.

if %error% == 0 (
    echo Shaders compiled ok
) else (
    echo There were shader compilation errors!
)

endlocal
exit /b

:CompileShader
set fxc=%PCFXC% %1.hlsl %FXCOPTS% /Tcs_4_0 /E%2 /Fhcompiled\%1_%2.inc /Fdcompiled\%1_%2.pdb /Vn%1_%2  /D ASPM_GPU /D ASPM_HLSL
echo.
echo %fxc%
%fxc% || set error=1
exit /b
