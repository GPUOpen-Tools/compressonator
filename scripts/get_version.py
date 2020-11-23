#!python
#
# simple script to read version information from Compressonator\Header\Version.h
# Usage
#   export WORKSPACE=<workspace-root>
#   set <var>=`python $WORKSPACE/compressonator/scripts/getversion.py --major|--minor`
#
import os
import argparse

# Get full path to script to support run from anywhere
scriptRoot = os.path.dirname(os.path.realpath(__file__))

# handle command line arguments
parser = argparse.ArgumentParser(description='Get GPA version information')
parser.add_argument('--major', action='store_true', default=False, help='Return value of MAJOR version string')
parser.add_argument('--minor', action='store_true', default=False, help='Return value of MINOR version string')
args = parser.parse_args()

# initialize file for search
gpaVersionFile = os.path.join(os.environ['WORKSPACE'], 'compressonator/cmp_compressonatorlib/version.h')
gpaVersionData = open(gpaVersionFile)

# get major, minor, and update values
for line in gpaVersionData:
    if 'define VERSION_MAJOR_MAJOR ' in line:
        major = (line.split()[2])
    if 'define VERSION_MAJOR_MINOR ' in line:
        minor = (line.split()[2])

#close file
gpaVersionData.close()

# print requested value
if args.major:
    print (major)
if args.minor:
    print (minor)
