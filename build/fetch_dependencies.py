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
# Assume workspace root is two folders up from scriptRoot (RGA/Build)
workspace = os.path.abspath(os.path.normpath(os.path.join(scriptRoot, "../..")))

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

gitMapping = {
 # Lib.
    "common-lib-amd-APPSDK-3.0.git"         : ["Common/Lib/AMD/APPSDK",     "master"],
    "common-lib-ext-Boost-1.59.git"         : ["Common/Lib/Ext/Boost",      "master"],
    ghRoot+"Catch2.git"                     : ["Common/Lib/Ext/Catch2",     "master"],
    "common-lib-ext-glew-1.9.git"           : ["Common/Lib/Ext/glew",       "master"],
    "common-lib-ext-OpenCV-2.49.git"        : ["Common/Lib/Ext/OpenCV",     "master"],
    ghRoot+"common-lib-ext-OpenEXR-2.2.git" : ["Common/Lib/Ext/OpenEXR",    "master"],
    "common-lib-ext-OpenGL.git"             : ["Common/Lib/Ext/OpenGL",     "master"],
    "common-lib-ext-tinyxml-2.6.2.git"      : ["Common/Lib/Ext/tinyxml",    "master"],
    "common-lib-ext-zlib-1.2.8.git"         : ["Common/Lib/Ext/zlib",       "master"]
}

# The following section contains OS-specific dependencies that are downloaded and placed in the specified target directory.
# key = GitHub release link
# value = location
downloadMappingWin = {
    "https://github.com/microsoft/DirectXTex/archive/jun2020b.zip" : "../../Common/Lib/Ext/DirectXTex/",
    "https://github.com/GPUOpen-LibrariesAndSDKs/OCL-SDK/files/1406216/lightOCLSDK.zip" : "../../Common/Lib/Ext/OpenCL/"
}
downloadMappingLin = {
    "https://github.com/microsoft/DirectXTex/archive/jun2020b.tar.gz" : "../../Common/Lib/Ext/DirectXTex/"
}

# for each dependency - test if it has already been fetched - if not, then fetch it, otherwise update it to top of tree
for key in gitMapping:
    # Target path, relative to workspace
    path = gitMapping[key][0]
    if ghRoot in key:
        # absolute path defined
        source = key
    else:
        source = gitRoot + key

    reqdCommit = gitMapping[key][1]
    # reqdCommit may be "None" - or user may override commit via command line. In this case, use tip of tree
    if((len(sys.argv) != 1 and sys.argv[1] == "latest") or reqdCommit is None):
        reqdCommit = gitBranch

    print("\nChecking out commit: " + reqdCommit + " for " + key)

    os.chdir(workspace)
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

# detect the OS
MACHINE_OS = ""
if "windows" in platform.system().lower():
    MACHINE_OS = "Windows"
elif "cygwin" in platform.system().lower():
    MACHINE_OS = "Windows"
elif "linux" in platform.system().lower():
    MACHINE_OS = "Linux"
else:
    print("Operating system not recognized correctly")
    sys.exit(1)

# reference the correct archive path
downloadMapping = ""
if MACHINE_OS == "Linux":
    downloadMapping = downloadMappingLin
else:
    downloadMapping = downloadMappingWin

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
