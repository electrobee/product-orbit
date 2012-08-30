#!/bin/sh

# Use "otool -L ProductOrbit" to see used libraries

RESOURCES=build/Release/ProductOrbit.app/Contents/Resources
EXES=build/Release/ProductOrbit.app/Contents/MacOS

cp /usr/local/lib/libusb-1.0.0.dylib $RESOURCES
cp /usr/local/lib/libusb-0.1.4.dylib $RESOURCES
cp /usr/local/lib/libjpeg.8.dylib $RESOURCES
install_name_tool -id @executable_path/../Resources/libusb-1.0.0.dylib $RESOURCES/libusb-1.0.0.dylib
install_name_tool -id @executable_path/../Resources/libusb-0.1.4.dylib $RESOURCES/libusb-0.1.4.dylib 
install_name_tool -id @executable_path/../Resources/libjpeg.8.dylib $RESOURCES/libjpeg.8.dylib
install_name_tool -change /usr/local/lib/libusb-1.0.0.dylib @executable_path/../Resources/libusb-1.0.0.dylib $RESOURCES/libusb-0.1.4.dylib
install_name_tool -change /usr/local/lib/libusb-0.1.4.dylib @executable_path/../Resources/libusb-0.1.4.dylib $EXES/ProductOrbit
install_name_tool -change /usr/local/lib/libjpeg.8.dylib @executable_path/../Resources/libjpeg.8.dylib $EXES/ProductOrbit

rm -Rf build/Release/ProductOrbit/ProductOrbit.app
rm -f build/Release/ProductOrbit/ProductOrbit.dmg
cp -Rf build/Release/ProductOrbit.app build/Release/ProductOrbit
ln -fs /Applications build/Release/ProductOrbit/Applications
create-dmg/create-dmg --window-size 500 300 --background background.png --icon-size 96 --volname "ProductOrbit" --icon "Applications" 380 205 --icon "ProductOrbit" 110 205 build/Release/ProductOrbit.dmg build/Release/ProductOrbit
