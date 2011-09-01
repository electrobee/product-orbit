# 1. Install CodeBlocks, MinGW and wxWidgets in the default locations
# 2. Build wxWidgets from cmd.exe:

SET PATH=c:\MinGW\bin;c:\MinGW\mingw32\bin

# Apply patch
patch -p0 < /c/Projects/Objectspin/trunk/wxWidgets-2.8.10.patch

# see config.gcc for default settings & more options
# clean before building
mingw32-make -f makefile.gcc MONOLITHIC=0 SHARED=0 UNICODE=1 BUILD=release clean

# build non-monolithic statically linked libraries with unicode support
mingw32-make -f makefile.gcc MONOLITHIC=0 SHARED=0 UNICODE=1 BUILD=release

# clean & build debug libs
mingw32-make -f makefile.gcc MONOLITHIC=0 SHARED=0 UNICODE=1 BUILD=debug clean
mingw32-make -f makefile.gcc MONOLITHIC=0 SHARED=0 UNICODE=1 BUILD=debug

# 3. Download Boost C++ libraries and extract at the same level as trunk
# 5. On Windows: Open win/objectspin.cbp. Make sure MinGW is set as the compiler.

----------------------------

Mac OS X Instructions

in wxMac-2.8.10 run
mkdir osx-debug && cd osx-debug
../configure --enable-debug --disable-shared --enable-unicode CFLAGS="-arch i386" CXXFLAGS="-arch i386" CPPFLAGS="-arch i386" LDFLAGS="-arch i386" OBJCFLAGS="-arch i386" OBJCXXFLAGS="-arch i386" --with-macosx-sdk=/Developer/SDKs/MacOSX10.5.sdk --with-macos-version-min=10.5
make

in wxMac-2.8.10 run
mkdir osx-release && cd osx-release
../configure --disable-shared --enable-unicode CFLAGS="-arch i386" CXXFLAGS="-arch i386" CPPFLAGS="-arch i386" LDFLAGS="-arch i386" OBJCFLAGS="-arch i386" OBJCXXFLAGS="-arch i386" --with-macosx-sdk=/Developer/SDKs/MacOSX10.5.sdk --with-macos-version-min=10.5
make

In Xcode -> Preferences -> Source Trees add
WX_HEADERS=~/Source/wxMac-2.8.10/include
WXDEBUG_CFLAGS=`osx-debug/wx-config --cflags`
WXDEBUG_LFLAGS=`osx-debug/wx-config --libs`
WXRELEASE_CFLAGS=`osx-release/wx-config --cflags`
WXRELEASE_LFLAGS=`osx-release/wx-config --libs`
Well, those should be the outputs of the wx-config tool, obviously.