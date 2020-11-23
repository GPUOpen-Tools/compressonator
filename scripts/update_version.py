#!python
#
# Script to update BUILD value using BUILD_NUMBER environment variable
#
# Usage
#   export WORKSPACE=<jenkins-workspace-root>
#   python $WORKSPACE/main/Compressonator/CMP_CompressonatorLib/UpdateVersion.py <major> <minor> <build-number>
#
import os
import argparse

# Get full path to script to support run from anywhere
scriptRoot = os.path.dirname(os.path.realpath(__file__))

# handle command line arguments
parser = argparse.ArgumentParser('Update version string in version.h')
parser.add_argument("major", help='VERSION_MAJOR_MAJOR value')
parser.add_argument("minor", help='VERSION_MAJOR_MINOR value')
parser.add_argument("buildnumber", help='VERSION_MINOR_MAJOR value')
#parser.add_argument("update", help='VERSION_MINOR_MINOR value')
updateArgs = parser.parse_args()

# initialize file for search
cmpVersionFile = os.path.join(os.environ['WORKSPACE'], 'compressonator/cmp_compressonatorlib/version.h')
cmpVersionData = open(cmpVersionFile, 'r')

# replace version string in data and write back out to file
newData = []
for line in cmpVersionData:
    if 'define VERSION_MAJOR_MAJOR' in line:
        newline = "#define VERSION_MAJOR_MAJOR " + updateArgs.major + "\n"
        newData.append(newline)
    elif 'define VERSION_MAJOR_MINOR' in line:
        newline = "#define VERSION_MAJOR_MINOR " + updateArgs.minor + "\n"
        newData.append(newline)
    elif 'define VERSION_MINOR_MAJOR' in line:
        newline = "#define VERSION_MINOR_MAJOR " + updateArgs.buildnumber + "\n"
        newData.append(newline)
#    elif 'define VERSION_MINOR_MINOR' in line:
#        newline = "#define VERSION_MINOR_MINOR " + updateArgs.update + "\n"
#        newData.append(newline)
    elif 'define VERSION_TEXT ' in line:
        newline = '#define VERSION_TEXT \"' + updateArgs.major + ', ' + updateArgs.minor + ', ' + updateArgs.buildnumber + '\"' + "\n"
        newData.append(newline)
    elif 'define VERSION_TEXT_SHORT' in line:
        newline = '#define VERSION_TEXT_SHORT \"' + updateArgs.major + "." + updateArgs.minor + '\"' + "\n"
        newData.append(newline)
    else:
        newData.append(line)
cmpVersionData.close()
cmpVersionData = open(cmpVersionFile, 'w')
cmpVersionData.writelines(newData)
cmpVersionData.close()


