Installing wxWidgets-2.9.4 on OS X
==================================
mkdir -p ~/src/electrobee
cd ~/src/electrobee
wget http://downloads.sourceforge.net/project/wxwindows/2.9.4/wxWidgets-2.9.4.tar.bz2
tar xjvf wxWidgets-2.9.4.tar.bz2
cd wxWidgets-2.9.4
mkdir osx-debug && cd osx-debug
../configure --enable-debug --disable-shared --enable-unicode --with-osx_cocoa --disable-std_iostreams --disable-printarch --with-expat=builtin --enable-universal-binary=i386 CFLAGS="-arch i386" CXXFLAGS="-arch i386" CPPFLAGS="-arch i386" LDFLAGS="-arch i386" OBJCFLAGS="-arch i386" OBJCXXFLAGS="-arch i386"
cd ~/src/electrobee
mkdir osx-release && cd osx-release
../configure --disable-shared --enable-unicode --with-osx_cocoa --disable-std_iostreams --disable-printarch --with-expat=builtin --enable-universal-binary=i386 CFLAGS="-arch i386" CXXFLAGS="-arch i386" CPPFLAGS="-arch i386" LDFLAGS="-arch i386" OBJCFLAGS="-arch i386" OBJCXXFLAGS="-arch i386"

Installing boost_1_51 on OS X
=============================
cd ~/src/electrobee
wget http://sourceforge.net/projects/boost/files/boost/1.51.0/boost_1_51_0.tar.bz2
tar xjvf boost_1_51_0.tar.bz2 


Installing libusb on OS X
=========================
brew install libusb --universal --HEAD
brew install ~/src/electrobee/product-orbit/mac/libusb-compat.rb --universal --HEAD

Installing ImageMagick on OS X
==============================
cd ~/src/electrobee
wget http://www.imagemagick.org/download/ImageMagick-6.7.9-0.tar.bz2
tar xjvf ImageMagick-6.7.9-0.tar.bz2
cd ImageMagick-6.7.9-0
wget http://www.imagemagick.org/download/delegates/libpng-1.5.12.tar.gz
tar xvzf libpng-1.5.12.tar.gz 
mv libpng-1.5.12 png
cd png && ./configure --disable-shared && make && cd ..
wget http://www.imagemagick.org/download/delegates/jpegsrc.v8b.tar.gz
tar xzvf jpegsrc.v8b.tar.gz
mv jpeg-8b jpeg
cd jpeg && ./configure --disable-shared && make && cd ..
wget http://www.imagemagick.org/download/delegates/zlib-1.2.7.tar.bz2
tar xzvf zlib-1.2.7.tar.bz2 
mv zlib-1.2.7 zlib
cd jpeg && ./configure --static && make && cd ..
/configure --disable-osx-universal-binary --without-perl --disable-shared --enable-static --without-gslib --without-x --without-lcms --without-tiff --without-xml --without-bzlib --without-jp2 --without-lzma --disable-opencl --disable-openmp --enable-zero-configuration --disable-installed --enable-delegate-build
cp utilities/convert /Users/alexei/src/electrobee/product-orbit/mac/magick