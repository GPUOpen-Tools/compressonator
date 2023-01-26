@echo off
rem Copyright (c) AMD. All rights reserved.
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

if exist CAS_main.hlsl (
echo ================
echo  CAS
echo ================
call :compileFXshader CAS_main.hlsl CompiledShader_NoScaling0_Linear0 false false
call :compileFXshader CAS_main.hlsl CompiledShader_NoScaling1_Linear0 true  false
call :compileFXshader CAS_main.hlsl CompiledShader_NoScaling0_Linear1 false true
call :compileFXshader CAS_main.hlsl CompiledShader_NoScaling1_Linear1 true  true
)

if exist fsr_pass.hlsl (
echo ===========================================================================
echo  FSR : SAMPLE_SLOW_FALLBACK SAMPLE_BILINEAR SAMPLE_RCAS SAMPLE_EASU 
echo ===========================================================================
call :compileFSRshader fsr_pass.hlsl fsr_easu     1 0 0 1
call :compileFSRshader fsr_pass.hlsl fsr_rcas     1 0 1 0
call :compileFSRshader fsr_pass.hlsl fsr_bilinear 1 1 0 0
)

echo.

if %error% == 0 (
    echo shaders compiled ok
) else (
    echo There were shader compilation errors!
)

endlocal
exit /b

:compileFXshader
set fxc=%PCFXC% /T cs_5_0 /E mainCS /O3 /Fh compiled\%2.h %1 /D ASPM_GPU /D ASPM_HLSL /D NO_SCALING=%3 /D LINEAR=%4
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:compileFSRshader
set fxc=%PCFXC% /T cs_5_0 /E mainCS /O3 /Fh compiled\%2.h %1 /D WIDTH=64 /D HEIGHT=1 /D DEPTH=1 /D SAMPLE_SLOW_FALLBACK=%3 /D SAMPLE_BILINEAR=%4 /D SAMPLE_RCAS=%5 /D SAMPLE_EASU=%6 
echo.
echo %fxc%
%fxc% || set error=1
exit /b
