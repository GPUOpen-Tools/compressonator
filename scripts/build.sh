#!/usr/bin/env bash

COMPRESSONATOR_PROJECT_DIR="$( cd $( cd "$(dirname "$0")" ; pwd -P )/../ ; pwd -P )"

BUILD_DIR=${COMPRESSONATOR_PROJECT_DIR}/build
INSTALL_DIR=${COMPRESSONATOR_PROJECT_DIR}/build-out
DEPENDENCIES_BUILD_DIR=${COMPRESSONATOR_PROJECT_DIR}/External/build
DEPENDENCIES_INSTALL_DIR=${COMPRESSONATOR_PROJECT_DIR}/External/build-out
DEPENDENCIES_CMAKE_DIR=${COMPRESSONATOR_PROJECT_DIR}/External/cmake

# Env Defaults
if [ "$(uname)" == "Darwin" ]; then
    export CMAKE_GENERATOR="Xcode"
fi

# Defaults
CLEAN=0
COMPILE_DEPS_ONLY=0
COMPILE_DEPS=0
LIBRARY_PATHS=()
TEST=0
TEST_ONLY=0
INSTALL=0
INSTALL_ONLY=0

function compile-dependencies() {

    local clean_build="OFF"
    if [[ ${CLEAN} = 1 ]]; then
        clean_build="ON"
    fi

    # Create our build dir
    mkdir -p "$@"

    # Add the build dir to the stack
    pushd "$@"

    # Configure
    cmake ${DEPENDENCIES_CMAKE_DIR} \
    -DCOMPRESSONATOR_PROJECT_DIR:STRING="${COMPRESSONATOR_PROJECT_DIR}" \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCLEAN_BUILD:BOOL=${clean_build}
    ${_EXTRA_CMAKE_FLAGS}

    # Build all targets
    cmake --build . --config Release
    cmake --install . --config Release

    # Pop build dir
    popd
}

function build-project() {

    # Create our build dir
    mkdir -p "$@"

    # Add the build dir to the stack
    pushd "$@"

    # Configure CMake
    cmake ${COMPRESSONATOR_PROJECT_DIR} \
        -DCOMPRESSONATOR_PROJECT_DIR="${COMPRESSONATOR_PROJECT_DIR}" \
        -DCMAKE_BUILD_TYPE:STRING="Release" \
        ${_EXTRA_CMAKE_FLAGS}

    # Build all targets
    cmake --build . --config Release

    # Pop build dir
    popd
}

function test-project() {
    # Add the build dir to the stack
    pushd "$@"

    # Run tests
    ctest -C Release

    # Pop build dir
    popd
}

function install-project() {
    # Add the build dir to the stack
    pushd "$@"

    # Run tests
    cmake --install . --config Release

    # Pop build dir
    popd
}

function print_help() {
    cat <<EOF
usage: Build.sh [-h] [--compile-deps COMPILE_DEPS] [--compile-deps-only COMPILE_DEPS_ONLY] [--clean CLEAN] [--lib LIB] [--test TEST] [--test-only TEST_ONLY] [--install INSTALL] [--install-only INSTALL_ONLY]

optional arguments:
  -h, --help            show this help message and exit
  --compile-deps COMPILE_DEPS, -cd COMPILE_DEPS
                        Compile external dependencies
  --compile-deps-only COMPILE_DEPS_ONLY, -cdo COMPILE_DEPS_ONLY
                        Only compile external dependencies
  --clean CLEAN         Perform a clean action (delete existing buildartifacts)
  --lib LIB, -l LIB     Additional library paths
  --test TEST, -t TEST  Run ctest (release config) on built project
  --test-only TEST_ONLY, -to TEST_ONLY
                        Only run ctest (release config) on built project
  --install INSTALL     Install artifacts into the build-out director
  --install-only INSTALL_ONLY
                        Only install artifacts into the build-out director
EOF
}

POSITIONAL=()
while [[ $# -gt 0 ]]
do
    key="$1"

    case $key in
        -help | --help | -h)
            print_help 
            exit 0
        ;;
        --compile-deps | -cd)
            COMPILE_DEPS=1
            shift # past argument
        ;;
        --compile-deps-only | -cdo)
            COMPILE_DEPS=1
            COMPILE_DEPS_ONLY=1
            shift # past argument
        ;;
        --lib | -l | -L)
            LIBRARY_PATHS+=("$2")
            shift # past argument
            shift # past value
        ;;
        --clean)
            CLEAN=1
            shift # past argument
        ;;
        --test | -t)
            TEST=1
            shift # past argument
        ;;
        --test-only)
            TEST=1
            TEST_ONLY=1
            shift # past argument
        ;;
        --install)
            INSTALL=1
            shift # past argument
        ;;
        --install-only)
            INSTALL=1
            INSTALL_ONLY=1
            shift # past argument
        ;;
        *)    # unknown option
            POSITIONAL+=("$1") # save it in an array for later
            shift # past argument
        ;;
esac
done

set -- "${POSITIONAL[@]}" # restore positional parameters

if [[ "${#POSITIONAL[@]}" -ne 0 ]]; then
    echo "Unexpected arguments: ${POSITIONAL}"
    echo ""
    print_help
    exit 1
fi

# Generate extra cmake flags
_EXTRA_CMAKE_FLAGS=""

# CMAKE_FIND_ROOT_PATH
if [[ "${#LIBRARY_PATHS[@]}" -ne 0 ]]; then
    for i in "${LIBRARY_PATHS[@]}"
    do
        LIBRARY_PATHS_STR="${i};${LIBRARY_PATHS_STR}"
    done
    _EXTRA_CMAKE_FLAGS+=" -DCMAKE_FIND_ROOT_PATH=${LIBRARY_PATHS_STR}"
fi

# Main functionality
set -ex

# Load an environment file if one exists
if [[ -f "${COMPRESSONATOR_PROJECT_DIR}/.env" ]]; then
    . ${COMPRESSONATOR_PROJECT_DIR}/.env
fi

# Pull submodules
if [[ ! -f ${COMPRESSONATOR_PROJECT_DIR}/.dependencies-initialized ]]; then
    git submodule update --init --recursive
    touch ${COMPRESSONATOR_PROJECT_DIR}/.dependencies-initialized
fi

# Optionally clean the build directory
echo "Clean Builds"
if [[ ${CLEAN} -eq 1 ]]; then
    echo "Cleaning build dirs"
    rm -rf "${BUILD_DIR}"
    rm -rf "${INSTALL_DIR}"

    if [[ ${COMPILE_DEPS} -eq 1 ]]; then
        echo "Cleaning external build dirs"
        rm -rf "${DEPENDENCIES_BUILD_DIR}"
        rm -rf "${DEPENDENCIES_INSTALL_DIR}"
    fi
fi

# If there is no dependency install directory, dependencies must be built
if [[ ! -d ${DEPENDENCIES_INSTALL_DIR} ]]; then
    COMPILE_DEPS=1
fi

# Optionally compile dependencies
echo "Build Dependencies"
if [[ ${COMPILE_DEPS} -eq 1 ]]; then
    compile-dependencies ${DEPENDENCIES_BUILD_DIR}
fi

# Create our build dir
echo "Build Project"
if [[ ${COMPILE_DEPS_ONLY} -eq 0 ]]; then

    if [[ ( ${TEST_ONLY} -ne 0 && ${INSTALL_ONLY} -ne 0 ) || ! -d ${BUILD_DIR} ]]; then
        build-project ${BUILD_DIR}
    fi

    echo "Run Tests"
    if [[ ${TEST} -eq 1 ]]; then
        test-project ${BUILD_DIR}
    fi

    echo "Install"
    if [[ ${INSTALL} -eq 1 ]]; then
        install-project ${BUILD_DIR}
    fi
fi