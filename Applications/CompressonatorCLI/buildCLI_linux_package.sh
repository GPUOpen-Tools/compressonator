#please make sure all the prerequite packages - initsetup_ubuntu.sh are installed and build-buildCLI_ubuntu_cmake.sh is run successfully before running this script. 
set -x
set -e

rm -rf Compressonator_Linux_x86_64_4.0*

mkdir Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp CompressonatorCLI-bin Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp CompressonatorCLI Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libboost_filesystem.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.58.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libboost_system.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libjasper.so.1* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libicui18n.so.55* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libicuuc.so.55* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libwebp.so.5* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libvpx.so.3* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libicudata.so.55* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libgtkglext-x11-1.0.so.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libgtkglext-x11-1.0.so.0.0.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libgdkglext-x11-1.0.so.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libgdkglext-x11-1.0.so.0.0.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libGLEW.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libGLEW.so.1.13 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libGLEW.so.1.13.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/local/ilmbase22build/lib/libHalf.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libHalf.la Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libHalf.so.12 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libHalf.so.12.0.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/local/ilmbase22build/lib/libIex-2_2.la Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libIex-2_2.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libIex-2_2.so.12 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libIex-2_2.so.12.0.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/local/ilmbase22build/lib/libIexMath-2_2.la Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libIexMath-2_2.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libIexMath-2_2.so.12 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libIexMath-2_2.so.12.0.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/local/ilmbase22build/lib/libImath-2_2.la Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libImath-2_2.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libImath-2_2.so.12 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libImath-2_2.so.12.0.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/local/ilmbase22build/lib/libIlmThread-2_2.la Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libIlmThread-2_2.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libIlmThread-2_2.so.12 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/ilmbase22build/lib/libIlmThread-2_2.so.12.0.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/local/openexr22build/lib/libIlmImf-2_2.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/openexr22build/lib/libIlmImf-2_2.so.22 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/local/openexr22build/lib/libIlmImf-2_2.so.22.0.0 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libdc1394.so* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libavcodec-ffmpeg.so* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libavformat-ffmpeg.so* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libavutil-ffmpeg.so* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libswscale-ffmpeg.so.3* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libswresample-ffmpeg.so.1* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libtbb.so.2* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libva.so.1* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libzvbi.so.0* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libxvidcore.so.4* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libx265.so.79* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libx264.so.148* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libtwolame.so.0* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libsnappy.so.1* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libshine.so.3* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libschroedinger-1.0.so.0* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libopenjpeg.so.5* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libmp3lame.so.0* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libgsm.so.1* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libcrystalhd.so.3* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libssh-gcrypt.so.4* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libmodplug.so.1* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libgme.so.0* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libbluray.so.1* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libsoxr.so.0* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libboost* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libjpeg* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libpng12* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libpangox* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libpcre16* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libopencv_core.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.9 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libopencv_highgui.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.9 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.9 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libz.a Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libpthread.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libQt5Core.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libQt5Core.so.5 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.5 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.5.1 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

cp /usr/lib/x86_64-linux-gnu/libQt5Gui.so Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5.5 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5.5.1 Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER
cp /usr/lib/x86_64-linux-gnu/qt5/plugins/imageformats/* Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER

tar -zcvf Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER.tar.gz Compressonator_Linux_x86_64_4.0.$BUILD_NUMBER










