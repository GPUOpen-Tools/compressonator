#! /usr/bin/python
#
# Simple script to update a set of common directories that are needed as dependencies of the current project
# Usage:
#   FetchDependencies.py [latest]
#
# If "latest" is specified, the latest commit will be checked out.
# Otherwise, the repos will be updated to the commit specified in the "gitMapping" table.
# If the required commit in the "gitMapping" is None, the repo will be updated to the latest commit.

import os
import subprocess
import sys
import urllib
import zipfile
import tarfile
import platform

isPython3OrAbove = None
if sys.version_info[0] >= 3:
    isPython3OrAbove = True

if isPython3OrAbove:
    import urllib.request

# to allow the script to be run from anywhere - not just the cwd - store the absolute path to the script file
scriptRoot = os.path.dirname(os.path.realpath(__file__))
# root is one folders up from scriptRoot (compressonator/scripts)
CompressonatorRoot = os.path.abspath(os.path.normpath(os.path.join(scriptRoot, "../")))

# When running this script on Windows (and not under cygwin), we need to set the shell=True argument to Popen and similar calls
# Without this option, Jenkins builds fail to find the correct version of git
SHELLARG = False
if ( sys.platform.startswith("win32")):
    # running on windows under default shell
    SHELLARG = True

# Print the version of git being used. This also confirms that the script can find git
try:
     subprocess.call(["git","--version"], shell=SHELLARG)
except OSError:
    # likely to be due to inability to find git command
    print("Error calling command: git --version")

# Calculate the root of the git server - all dependencies should be retrieved from the same server
gitURL = subprocess.check_output(["git", "-C", scriptRoot, "remote", "get-url", "origin"], shell=SHELLARG)
# Strip everything after the last '/' from the URL to retrieve the root
if gitURL == None:
    print("Error: Unable to determine origin for RGA git project")
    exit(1)
gitUrlStr = (str(gitURL).lstrip("b'"))
gitRoot = (gitUrlStr.rsplit('/',1))[0] + '/'

# If cloning from github - use the master branch - otherwise use amd-master
gitBranch = "amd-master"
if "github" in gitUrlStr:
    gitBranch = "master"

print("\nFetching dependencies from: " + gitRoot + " - using branch: " + gitBranch)

# Define a set of dependencies that exist as separate git projects. The parameters are:
# "git repo name"  : ["Directory for clone relative to this script",  "branch or commit to checkout (or None for top of tree)"

# Some repos are only hosted on github - these are defined with an absolute URL based here
ghRoot = "https://github.com/GPUOpen-Tools/"

# source reviews for use
#   "https://github.com/opencv/opencv.git"      : ["../common/lib/ext/opencv2",  "2.4.9"]
#   "https://github.com/openexr/openexr.git"    : ["../common/lib/ext/openexr", "master"],
#   "https://github.com/madler/zlib"            : ["../common/lib/ext/zlib",    "master"],
#   "https://github.com/catchorg/Catch2.git"    : ["../common/lib/ext/catch",   "master"],
#   "https://github.com/syoyo/tinyexr"          : ["../common/lib/ext/tinyexr", "master"],
#   "https://github.com/google/draco/tree/master/src/draco" : : ["../common/lib/ext/draco", "master"],

# Libs to build on Window
gitMappingWin = {
     ghRoot+"Catch2.git"                         : ["../common/lib/ext/catch2",     "master"],
     ghRoot+"common_lib_ext_glew_1.9.git"        : ["../common/lib/ext/glew",       "master"],
     ghRoot+"common_lib_ext_opencv_2.49.git"     : ["../common/lib/ext/opencv",     "master"],
     ghRoot+"common_lib_ext_openexr_2.2.git"     : ["../common/lib/ext/openexr",    "master"],
     ghRoot+"common_lib_ext_opengl.git"          : ["../common/lib/ext/opengl",     "master"],
     ghRoot+"common_lib_ext_tinyxml_2.6.2.git"   : ["../common/lib/ext/tinyxml",    "master"],
     ghRoot+"common_lib_ext_zlib_1.2.10.git"     : ["../common/lib/ext/zlib",       "master"],
     "https://github.com/g-truc/glm.git"         : ["../common/lib/ext/glm",        "0.9.8.0"],
     "https://github.com/discord/rapidxml.git"   : ["../common/lib/ext/rapidxml",   "master"],
     "https://github.com/KhronosGroup/KTX-Software.git" : ["../common/lib/ext/ktx", "v4.0.0-beta4"],
     "https://github.com/apitrace/dxsdk"         : ["../common/lib/ext/apitrace/dxsdk", "master"],
}

# Libs to build on Linux
gitMappingLin = {
     ghRoot+"common_lib_ext_openexr_2.2.git"     : ["../common/lib/ext/openexr",    "master"],
     "https://github.com/g-truc/glm.git"         : ["../common/lib/ext/glm",        "0.9.8.0"],
     "https://github.com/discord/rapidxml.git"   : ["../common/lib/ext/rapidxml",   "master"],
}

# Libs to build on Unix
gitMappingUni = {
     ghRoot+"common_lib_ext_openexr_2.2.git"     : ["../common/lib/ext/openexr",    "master"],
     "https://github.com/g-truc/glm.git"         : ["../common/lib/ext/glm",        "0.9.8.0"],
     "https://github.com/discord/rapidxml.git"   : ["../common/lib/ext/rapidxml",   "master"],
}

# The following section contains OS-specific dependencies that are downloaded and placed in the specified target directory.
# key = GitHub release link
# value = location
downloadMappingWin = {
    "https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.bin.WIN64.zip" : "../../common/lib/ext/glfw/",
    "https://github.com/microsoft/DirectXTex/archive/jun2020b.zip" : "../../common/lib/ext/directxtex/",
    "https://github.com/GPUOpen-LibrariesAndSDKs/OCL-SDK/files/1406216/lightOCLSDK.zip" : "../../common/lib/ext/opencl/",
}

downloadMappingLin = {
    "http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.2.0.tar.gz": "../../common/lib/ext/openexr2/ilmbase",
    "http://download.savannah.nongnu.org/releases/openexr/openexr-2.2.0.tar.gz": "../../common/lib/ext/openexr2/openexr",
}

downloadMappingUni = {
    "http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.2.0.tar.gz": "../../common/lib/ext/openexr2/ilmbase",
    "http://download.savannah.nongnu.org/releases/openexr/openexr-2.2.0.tar.gz": "../../common/lib/ext/openexr2/openexr",
}

# detect the OS
MACHINE_OS = ""
print("OS:",platform.system().lower())
if "windows" in platform.system().lower():
    MACHINE_OS = "WINDOWS"
elif "cygwin" in platform.system().lower():
    MACHINE_OS = "WINDOWS"
elif "linux" in platform.system().lower():
    MACHINE_OS = "LINUX"
elif "darwin" in platform.system().lower():
    MACHINE_OS = "UNIX"
else:
    print("Operating system not recognized correctly")
    sys.exit(1)
print("BUILD MACHINE AS:",MACHINE_OS)

# reference the correct archive path
gitMapping = ""
downloadMapping = ""
if MACHINE_OS == "LINUX":
    gitMapping = gitMappingLin
    downloadMapping = downloadMappingLin
if MACHINE_OS == "WINDOWS":
    gitMapping = gitMappingWin
    downloadMapping = downloadMappingWin
if MACHINE_OS == "UNIX":
    gitMapping = gitMappingUni
    downloadMapping = downloadMappingUni

# for each dependency - test if it has already been fetched - if not, then fetch it, otherwise update it to top of tree
def downloadgit(key, path, reqdCommit):
    # Target path, relative to workspace
    path   = gitMapping[key][0]
    source = key
    reqdCommit = gitMapping[key][1]
    # reqdCommit may be "None" - or user may override commit via command line. In this case, use tip of tree
    if((len(sys.argv) != 1 and sys.argv[1] == "latest") or reqdCommit is None):
        reqdCommit = gitBranch

    print("\nChecking out commit: " + reqdCommit + " for " + key)

    os.chdir(CompressonatorRoot)
    if os.path.isdir(path):
        # directory exists - get latest from git using pull
        print("Directory " + path + " exists. \n\tUsing 'git fetch' to get latest from " + source)
        sys.stdout.flush()
        try:
            subprocess.check_call(["git", "-C", path, "fetch", "origin"], shell=SHELLARG)
        except subprocess.CalledProcessError as e:
            print ("'git fetch' failed with return code: %d\n" % e.returncode)
        try:
            subprocess.check_call(["git", "-C", path, "checkout", reqdCommit], shell=SHELLARG)
        except subprocess.CalledProcessError as e:
            print ("'git checkout' failed with return code: %d\n" % e.returncode)
        sys.stderr.flush()
        sys.stdout.flush()
    else:
        # directory doesn't exist - clone from git
        print("Directory " + path + " does not exist. \n\tUsing 'git clone' to get latest from " + source)
        sys.stdout.flush()
        try:
            subprocess.check_call(["git", "clone", source, path], shell=SHELLARG)
            subprocess.check_call(["git", "-C", path, "checkout", reqdCommit], shell=SHELLARG)
        except subprocess.CalledProcessError as e:
            print ("'git clone' failed with return code: %d\n" % e.returncode)
            sys.exit(1)
        sys.stderr.flush()
        sys.stdout.flush()
for key in gitMapping:
    downloadgit(key, gitMapping[key][0], gitMapping[key][1])

# routine for downloading and unzipping an archive
def downloadandunzip(key, value):
    # convert targetPath to OS specific format
    tmppath = os.path.join(scriptRoot, "", value)
    # clean up path, collapsing any ../ and converting / to \ for Windows
    targetPath = os.path.normpath(tmppath)
    if False == os.path.isdir(targetPath):
        os.makedirs(targetPath)
    zipfileName = key.split('/')[-1].split('#')[0].split('?')[0]
    zipPath = os.path.join(targetPath, zipfileName)
    if False == os.path.isfile(zipPath):
        print("\nDownloading " + key + " into " + zipPath)
        if isPython3OrAbove:
            urllib.request.urlretrieve(key, zipPath)
        else:
            urllib.urlretrieve(key, zipPath)
        if os.path.splitext(zipPath)[1] == ".zip":
            zipfile.ZipFile(zipPath).extractall(targetPath)
            os.remove(zipPath)
        elif os.path.splitext(zipPath)[1] == ".gz":
            tarfile.open(zipPath).extractall(targetPath)
            os.remove(zipPath)

# for each archived release, download and unzip the artifacts into the target location

for key in downloadMapping:
    downloadandunzip(key, downloadMapping[key])
