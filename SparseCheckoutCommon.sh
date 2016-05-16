#!/bin/bash 
mkdir Common
cd Common
git init
git remote add -f origin https://github.com/GPUOpen-Tools/CxlCommon.git
git config core.sparseCheckout true

echo "Lib/AMD/APPSDK/3-0/*" >> .git/info/sparse-checkout
echo "Lib/Ext/Boost/boost_1_59_0/boost/*" >> .git/info/sparse-checkout
echo "Lib/Ext/Boost/boost_1_59_0/lib/VC14/x86/*" >> .git/info/sparse-checkout
echo "Lib/Ext/OpenEXR/v1.4.0/*" >> .git/info/sparse-checkout
echo "Lib/Ext/OpenCV/2.49/*" >> .git/info/sparse-checkout
echo "Lib/Ext/Qt/5.5/*" >> .git/info/sparse-checkout
echo "Lib/Ext/Qt/Global-QT.props" >> .git/info/sparse-checkout
echo "Lib/Ext/tinyxml/*" >> .git/info/sparse-checkout
echo "Lib/Ext/zlib/1.2.8/*" >> .git/info/sparse-checkout

git pull origin master