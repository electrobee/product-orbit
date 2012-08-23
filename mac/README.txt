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