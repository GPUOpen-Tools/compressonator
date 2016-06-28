#!/bin/bash

# get long description of vulkan sample applications
#  - currently displays descriptions for all samples that have set
#    a long description via the VULKAN_SAMPLE_DESCRIPTION_START and
#    VULKAN_SAMPLE_DESCRIPTION_END keywords
#
# usage:  get-descripts.sh
#
# TODO - support filename(s) options - display descripts for only those samples

# save command directory
CMDDIR="$(dirname "$(readlink -f ${BASH_SOURCE[0]})")"

# read all source .cpp files and display the long description
for f in $CMDDIR/*.cpp
do
#   DESCRIPT=`sed -n '/VULKAN_SAMPLE_DESCRIPTION_START/,/VULKAN_SAMPLE_DESCRIPTION_END/p' $f`
   DESCRIPT=`sed -n '/^VULKAN_SAMPLE_DESCRIPTION_START$/,/^VULKAN_SAMPLE_DESCRIPTION_END$/{ /^VULKAN_SAMPLE_DESCRIPTION_START/d; /^VULKAN_SAMPLE_DESCRIPTION_END/d; p; }' $f`
   if [ ! -z "$DESCRIPT" ]; then
       BNAME=`basename $f`
       echo "$BNAME:"
       echo "$DESCRIPT"
       echo ""
   fi
done

