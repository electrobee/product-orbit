Installing wxMac-2.8.10
=======================

Debug
-----

cd wxMac-2.8.10
mkdir osx-debug && cd osx-debug
../configure --enable-debug --disable-shared --enable-unicode CFLAGS="-arch i386" CXXFLAGS="-arch i386" CPPFLAGS="-arch i386" LDFLAGS="-arch i386" OBJCFLAGS="-arch i386" OBJCXXFLAGS="-arch i386"
make

Then in Xcode -> Preferences -> Source Trees add the following variables:

WXDEBUG_LFLAGS		output of osx-debug/wx-config --libs
WXDEBUG_CFLAGS		output of osx-debug/wx-config --cflags
WX_HEADERS 		something like /Users/alexei/Source/wxMac-2.8.10/include /Users/alexei/Source/wxMac-2.8.10/contrib/include 

Release
-------

cd wxMac-2.8.10
mkdir osx-release && cd osx-release
../configure --disable-shared --enable-unicode CFLAGS="-arch i386" CXXFLAGS="-arch i386" CPPFLAGS="-arch i386" LDFLAGS="-arch i386" OBJCFLAGS="-arch i386" OBJCXXFLAGS="-arch i386"
make

Then in Xcode -> Preferences -> Source Trees add the following variables:

WXRELEASE_LFLAGS	output of osx-release/wx-config --libs
WXRELEASE_CFLAGS	output of osx-release/wx-config --cflags



