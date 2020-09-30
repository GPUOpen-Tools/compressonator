#!/usr/bin/env pwsh

# Parameters
Param(
    [Parameter(Mandatory=$false)]
    [Alias("help", "h")]
    [switch]
    $PRINT_HELP,

    [Parameter(Mandatory=$false)]
    [Alias("compile-deps", "cd")]
    [switch]
    $COMPILE_DEPENDENCIES,

    [Parameter(Mandatory=$false)]
    [Alias("compile-deps-only", "cdo")]
    [switch]
    $COMPILE_DEPENDENCIES_ONLY,

    [Parameter(Mandatory=$false)]
    [Alias("clean")]
    [switch]
    $CLEAN,

    [Parameter(Mandatory=$false)]
    [Alias("test")]
    [switch]
    $TEST,

    [Parameter(Mandatory=$false)]
    [Alias("test-only", "to")]
    [switch]
    $TEST_ONLY,

    [Parameter(Mandatory=$false)]
    [Alias("install", "i")]
    [switch]
    $INSTALL,

    [Parameter(Mandatory=$false)]
    [Alias("install-only")]
    [switch]
    $INSTALL_ONLY,


    [Parameter(Mandatory=$false)]
    [Alias("library-path", "l")]
    [string[]]
    $LIBRARY_PATH
)

# Default variables --------------------------------------------------------------------------------------------------
$DEPENDENCIES_INIT_MARKER=".dependencies-initialized"
$COMPRESSONATOR_PROJECT_DIR=("$PSScriptRoot").Replace("\\", "/").Replace("\", "/")
$BUILD_DIR=("$PSScriptRoot/build").Replace("\\", "/").Replace("\", "/")

$DEPENDENCIES_BUILD_DIR=("$PSScriptRoot/External/build").Replace("\\","/").Replace("\", "/")
$DEPENDENCIES_INSTALL_DIR=("$PSScriptRoot/External/build-out").Replace("\\","/").Replace("\", "/")
$EXTERNALS_TARGETS_DIR=("$PSScriptRoot/External/cmake").Replace("\\","/").Replace("\", "/")

# If there is no variable "PSVersionTable" or the version is less than 6.0, we assume the platform is Windows
if ((!(Get-Variable PSVersionTable -Scope Global)) -or ([System.Version]$PSVersionTable.PSVersion -lt [System.Version]"6.0")) {
    $IS_WINDOWS=$true
    $IS_MACOS=$false
    $IS_LINUX=$false
} else {
    $IS_WINDOWS=$IsWindows
    $IS_MACOS=$IsMacOS
    $IS_LINUX=$IsLinux
}

if ($IS_WINDOWS) {
    $env:CMAKE_GENERATOR="Visual Studio 16 2019"
    $env:CMAKE_GENERATOR_PLATFORM="x64"
} elseif($IS_MACOS) {
    $env:CMAKE_GENERATOR="Xcode"
}

$CMAKE_BUILD_TYPE_RELEASE="Release"
$CMAKE_BUILD_TYPE_DEBUG="Debug"
    


# Functions -----------------------------------------------------------------------------------------------------------

function Print-Help {
    Write-Output @"
usage: Build.ps1 [-h] [--compile-deps COMPILE_DEPS] [--compile-deps-only COMPILE_DEPS_ONLY] [--clean CLEAN] [--lib LIB] [--test TEST] [--test-only TEST_ONLY] [--install INSTALL] [--install-only INSTALL_ONLY]

optional arguments:
    -h, --help            show this help message and exit
    -compile-deps COMPILE_DEPS, -cd COMPILE_DEPS
                        Compile external dependencies
    -compile-deps-only COMPILE_DEPS_ONLY, -cdo COMPILE_DEPS_ONLY
                        Only compile external dependencies
    -clean CLEAN         Perform a clean action (delete existing buildartifacts)
    -lib LIB, -l LIB     Additional library paths
    -test TEST, -t TEST  Run ctest (release config) on built project
    -test-only TEST_ONLY, -to TEST_ONLY
                        Only run ctest (release config) on built project
    -install INSTALL     Install artifacts into the build-out director
    -install-only INSTALL_ONLY
                        Only install artifacts into the build-out director
"@
}

function RM-Propper {
    function Remove-SymLinks {
        Get-ChildItem -Force -ErrorAction Stop @Args | Where-Object { if($_.Attributes -match "ReparsePoint"){$_.Delete()} }
    }
    Remove-SymLinks @Args -Recurse
    Remove-Item -Path @Args -Recurse -Force -ErrorAction Ignore
}

function Make-Dependencies {

    Param(
        [Parameter(Mandatory=$true)]
        [String]
        $TYPE
    )

    if ($CLEAN) {
        if (Test-Path $DEPENDENCIES_BUILD_DIR) {
            Write-Output "Cleaning $DEPENDENCIES_BUILD_DIR"
            RM-Propper $DEPENDENCIES_BUILD_DIR
        }
    }

    Write-Output "Building Dependencies"

    $DEPS_BUILD_DIR = $DEPENDENCIES_BUILD_DIR
    New-Item -ErrorAction Ignore -ItemType directory -Path $DEPS_BUILD_DIR

    $BUILD_SHARED_LIB_OPTIONS = @("UNSPECIFIED")
    if ($IS_WINDOWS) {
        $BUILD_SHARED_LIB_OPTIONS = @("ON", "OFF")
    }

    foreach ($option in $BUILD_SHARED_LIB_OPTIONS) {
        $CMAKE_SHARED_LIB_OPTION = ""
        if ($option -ne "UNSPECIFIED") {
            $CMAKE_SHARED_LIB_OPTION = "-DBUILD_SHARED_LIBS:BOOL=$option"
            if ($IS_WINDOWS) {
                $DEPS_BUILD_DIR = "$DEPENDENCIES_BUILD_DIR/$TYPE-shared-libs-$option"
            }
        }
        if (($CLEAN) -and (Test-Path $DEPS_BUILD_DIR)) {
            Write-Output "Cleaning $DEPS_BUILD_DIR"
            RM-Propper $DEPS_BUILD_DIR
        }
        New-Item -ErrorAction Ignore -ItemType directory -Path $DEPS_BUILD_DIR
        try {
            Push-Location -Path $DEPS_BUILD_DIR

            & cmake $EXTERNALS_TARGETS_DIR -DCOMPRESSONATOR_PROJECT_DIR:STRING="$COMPRESSONATOR_PROJECT_DIR" -DCMAKE_BUILD_TYPE:STRING="$TYPE" -DBUILD_SHARED_LIBS:BOOL="$option" $CMAKE_SHARED_LIB_OPTION
            if ($? -eq $false) { Write-Error "Exiting due to Configuration Error for Dependencies" }

            & cmake --build . --config $TYPE
            if ($? -eq $false) { Write-Error "Exiting due to $TYPE Compilation Error for Dependencies" }
        } finally {
            Pop-Location
        }
    }
}

function Build-Project {

    New-Item -ErrorAction Ignore -ItemType directory -Path $BUILD_DIR

    try {
        $BUILD_EXE_DIR=$BUILD_DIR
        $CMAKE_DIR=$COMPRESSONATOR_PROJECT_DIR
        
        Push-Location -Path $BUILD_EXE_DIR

        if ($LIBRARY_PATH) {
            $FIND_ROOT_PATHS = ([system.String]::Join(";", $LIBRARY_PATH)).Replace("\\", "/").Replace("\", "/")
            & cmake $CMAKE_DIR -DCOMPRESSONATOR_PROJECT_DIR="$COMPRESSONATOR_PROJECT_DIR" -DCMAKE_FIND_ROOT_PATH="$FIND_ROOT_PATHS"
        } else {
            & cmake $CMAKE_DIR -DCOMPRESSONATOR_PROJECT_DIR="$COMPRESSONATOR_PROJECT_DIR"
        }
        if ($? -eq $false) { Write-Error "Exiting due to Configuration Error" }

        & cmake --build . --config Release
        if ($? -eq $false) { Write-Error "Exiting due to Compilation Error" }
        
    } finally {
        Pop-Location
    }
}


function Test-Project {

    try {
        
        Push-Location -Path $BUILD_DIR

        & ctest -C Release
        if ($? -eq $false) { Write-Error "Tests failed"}
        
    } finally {
        Pop-Location
    }
}


function Install-Project {

    try {
        
        Push-Location -Path $BUILD_DIR

        & cmake --install . --config Release
        if ($? -eq $false) { Write-Error "Installation failed"}
        
    } finally {
        Pop-Location
    }
}

# Main functionality --------------------------------------------------------------------------------------------------
$ErrorActionPreference = "Stop"

if ($PRINT_HELP) {
    Print-Help
    exit 0
}

if (Test-Path "$COMPRESSONATOR_PROJECT_DIR/.env") {
    . "$COMPRESSONATOR_PROJECT_DIR/.env"
}

# Pull submodules
if (!(Test-Path "$PSScriptRoot\$DEPENDENCIES_INIT_MARKER")) {
    & git submodule update --init --recursive
    if ($? -eq $false) { Write-Error "Exiting due to Git Failure" }

    New-Item -Path $PSScriptRoot -Name $DEPENDENCIES_INIT_MARKER -ItemType "file" -Force
    if ($IS_WINDOWS) {
        (Get-Item -Path "$PSScriptRoot/$DEPENDENCIES_INIT_MARKER").Attributes += "Hidden"
    }
}

# If there is no dependency install directory, dependencies must be built
if (!(Test-Path $DEPENDENCIES_INSTALL_DIR)) {
    $COMPILE_DEPENDENCIES=$true
}

# Compile dependencies if required
if ($COMPILE_DEPENDENCIES -or $COMPILE_DEPENDENCIES_ONLY) {

    # Clean build
    if ($CLEAN -eq $true) {
        if (Test-Path $DEPENDENCIES_INSTALL_DIR) {
            Write-Output "Cleaning build dir: $DEPENDENCIES_INSTALL_DIR"
            RM-Propper $DEPENDENCIES_INSTALL_DIR
        }
    }

    # Compile debug dependencies for windows
    if ($IS_WINDOWS) {
        Make-Dependencies -Type $CMAKE_BUILD_TYPE_DEBUG
    }

    Make-Dependencies -Type $CMAKE_BUILD_TYPE_RELEASE
}

# Build the main project
if (!$COMPILE_DEPENDENCIES_ONLY) {

    # Clean build
    if ($CLEAN -eq $true) {
        if (Test-Path $BUILD_DIR) {
            Write-Output "Cleaning build dir: $BUILD_DIR"
            RM-Propper $BUILD_DIR
        }
    }

    if (($TEST_ONLY -eq $false && $INSTALL_ONLY -eq $false) || !(Test-Path $BUILD_DIR) {
        Build-Project
    }

    if ($TEST) {
        Test-Project
    }

    if ($INSTALL) {
        Install-Project
    }
}
