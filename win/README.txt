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