#please make sure all the prerequite packages - initsetup_ubuntu.sh are installed and build-buildCLI_ubuntu_cmake.sh is run successfully before running this script. 
set -x
set -e

cd $WORKSPACE/compressonator

rm -rf compressonatorcli_linux_x86_64_4.1*
mkdir compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER
mkdir compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/documents
mkdir compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/images
mkdir compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/license
mkdir compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/qt
mkdir compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs

cp scripts/compressonatorcli compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER
cp bin/compressonatorcli-bin compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER

cp -r docs/build/html compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/documents
cp -r runtime/images  compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER
cp license/license.txt compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/license

# Qt
cp $QT_ROOT/lib/libQt5Core.so* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/qt
cp $QT_ROOT/lib/libQt5Core.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/qt
cp $QT_ROOT/lib/libQt5Gui.so* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/qt
cp $QT_ROOT/lib/libQt5Gui.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/qt
cp $QT_ROOT/lib/libicui18n.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/qt
cp $QT_ROOT/lib/libicuuc.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/qt
cp $QT_ROOT/lib/libicudata.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/qt
cp $QT_ROOT/plugins/imageformats/* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/qt

# OpenCV
cp /usr/lib/x86_64-linux-gnu/libopencv_core.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/lib/x86_64-linux-gnu/libopencv_core.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/lib/x86_64-linux-gnu/libopencv_highgui.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs

# Misc
cp /usr/lib/x86_64-linux-gnu/libtbb.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs

# Optional User pkg update 
cp scripts/initsetup_ubuntu.sh compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs

# GLEW if needed
# cp /usr/lib/x86_64-linux-gnu/libGLEW.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libGLEW.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs

# OpenEXR if needed
cp /usr/local/ilmbase22build/lib/libHalf.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libHalf.la compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libHalf.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libIex-2_2.la compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libIex-2_2.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libIex-2_2.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libIexMath-2_2.la compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libIexMath-2_2.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libIexMath-2_2.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libImath-2_2.la compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libImath-2_2.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libImath-2_2.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libIlmThread-2_2.la compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libIlmThread-2_2.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/ilmbase22build/lib/libIlmThread-2_2.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/openexr22build/lib/libIlmImf-2_2.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
cp /usr/local/openexr22build/lib/libIlmImf-2_2.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs


# Misc runtime enable if needed in pkglibs
# cp /usr/lib/x86_64-linux-gnu/libwebp.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libvpx.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libdc1394.so* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libva.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libzvbi.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libxvidcore.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libx265.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libx264.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libtwolame.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libsnappy.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libshine.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libmp3lame.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libgsm.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libcrystalhd.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libssh-gcrypt.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libgme.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libbluray.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libsoxr.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libstdc++.so.* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libjpeg* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libpng* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libpangox* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libpcre16* compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libz.a compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs
# cp /usr/lib/x86_64-linux-gnu/libpthread.so compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER/pkglibs

tar -zcvf compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER.tar.gz compressonatorcli_linux_x86_64_4.1.$BUILD_NUMBER
