#!/bin/bash
set -x #echo on
HOME=/Users/OsleTek

#to force building ui files: $HOME/Qt/5.5/gcc_64/bin/uic MainWindow.ui > ui_MainWindow.h
#(on mac) ~/Qt/5.5/clang_64/bin/uic  MainWindow.ui > ui_MainWindow.h

#1 In CMakeLists.txt file set the correct PROJECT_NAME
#! In MainWindow.cpp, define or not define DEFINE_SPRITEMILL
#2 Compile the project in QtCreator (or cmake/build)
#! You should have ${PROJECT_NAME}.app in the build directory
#3 In resources/Info.plist file, change the icon name to SquiggleMark or SpriteMill (for windows, change squigglemark.rc)
#! Below change export APP to the SquiggleMark or SpriteMill
#4 In terminal, cd to the build directory, and run this script
#! ../SquiggleMark/install-scripts/macinstall.sh 
#! You should have the "$APP" directory containing "$APP".app and Applications symbolic link
#! Follow instructions at the end of this file

export QTDIR="$HOME/Qt5/5.5"
export APP="SquiggleMark"
#export APP="SpriteMill"
export BUNDLE="$APP".app

#echo $BUNDLE
#exit
#mv $HOME/Qt~ $HOME/Qt

#making icon that will show in title bar of the App
#Use online service such as http://www.convertico.com
#making app icon for MacOs
#mkdir -p $HOME/Desktop/work/SquiggleMark/resources/SquiggleMark.iconset
#drop icons into this folder
#icon_16x16.png
#icon_16x16@2x.png
#icon_32x32.png
#icon_32x32@2x.png
#icon_128x128.png
#icon_128x128@2x.png
#icon_256x256.png
#icon_256x256@2x.png
#icon_512x512.png
#icon_512x512@2x.png
#cd $HOME/Desktop/work/SquiggleMark/resources
#iconutil -c icns -o SquiggleMark.icns SquiggleMark.iconset
#iconutil -c icns -o SpriteMill.icns SpriteMill.iconset

#cd /Users/OsleTek/work/SquiggleMark-build
mkdir -p tmp
chmod +x $BUNDLE/Contents/MacOS/$APP
mkdir -p $BUNDLE/Contents/Frameworks
mkdir -p $BUNDLE/Contents/lib
mkdir -p $BUNDLE/Contents/Resources
mkdir -p $BUNDLE/Contents/MacOS/plugins/platforms
mkdir -p $BUNDLE/Contents/MacOS/plugins/imageformats
cp ../SquiggleMark/resources/$APP.icns $BUNDLE/Contents/Resources
cp ../SquiggleMark/resources/Info.plist $BUNDLE/Contents
#cp ../SquiggleMark/resources/"$APP"_license.key $BUNDLE/Contents/MacOS/license.key

rm -fr $APP

cp $QTDIR/clang_64/lib/QtWidgets.framework/Versions/5/QtWidgets $BUNDLE/Contents/Frameworks
cp $QTDIR/clang_64/lib/QtSvg.framework/Versions/5/QtSvg $BUNDLE/Contents/Frameworks
cp $QTDIR/clang_64/lib/QtGui.framework/Versions/5/QtGui $BUNDLE/Contents/Frameworks
cp $QTDIR/clang_64/lib/QtCore.framework/Versions/5/QtCore $BUNDLE/Contents/Frameworks
cp $QTDIR/clang_64/lib/QtDBus.framework/Versions/5/QtDBus $BUNDLE/Contents/Frameworks
cp $QTDIR/clang_64/lib/QtPrintSupport.framework/Versions/5/QtPrintSupport $BUNDLE/Contents/Frameworks
cp $QTDIR/clang_64/plugins/platforms/libqcocoa.dylib $BUNDLE/Contents/MacOS/plugins/platforms
cp $QTDIR/clang_64/plugins/imageformats/libqjpeg.dylib $BUNDLE/Contents/MacOS/plugins/imageformats

cp /usr/lib/libSystem.B.dylib $BUNDLE/Contents/MacOS/
cp  /usr/lib/libiconv.2.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libcrypto.1.1.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libssl.1.1.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libz.1.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libjpeg.9.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libpng16.16.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libgd/Bin/libgd.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libfreetype.6.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libavcodec.57.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libavdevice.57.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libavfilter.6.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libavformat.57.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libavutil.55.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libswresample.2.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libswscale.4.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libcurl.4.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libgd.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libvorbisenc.2.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libvorbis.0.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libogg.0.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libtheoraenc.1.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libtheoradec.1.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libopenh264.2.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/libmp3lame.0.dylib $BUNDLE/Contents/MacOS/
cp ../libs/build/osx/bin/ffmpeg $BUNDLE/Contents/MacOS/
cp /System/Library/Frameworks/AudioToolbox.framework/Versions/A/AudioToolbox $BUNDLE/Contents/MacOS/libAudioToolbox.dylib

##
rm -fr macbundle
mkdir macbundle
cp -R $BUNDLE macbundle
ln -s /Applications/ Applications
mv Applications macbundle
rm -fr "$APP"
mv macbundle "$APP"
#read -p "Press [Enter] to continue..."

rm -f macbundle.dmg
rm -f $APP.dmg

#sleep 2
#read -p "Press [Enter] to continue..."
install_name_tool -id @executable_path/$APP ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -delete_rpath $QTDIR/clang_64/lib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change @rpath/QtSvg.framework/Versions/5/QtSvg @executable_path/../Frameworks/QtSvg ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change @rpath/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change @rpath/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change @rpath/QtDBus.framework/Versions/5/QtDBus @executable_path/../Frameworks/QtDBus ./$APP/$APP.app/Contents/MacOS/$APP

#/$APP
install_name_tool -change $QTDIR/clang_64/lib/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change $QTDIR/clang_64/lib/QtSvg.framework/Versions/5/QtSvg @executable_path/../Frameworks/QtSvg ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change $QTDIR/clang_64/lib/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change $QTDIR/clang_64/lib/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change $QTDIR/clang_64/lib/QtPrintSupport.framework/Versions/5/QtPrintSupport @executable_path/../Frameworks/QtPrintSupport ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change $QTDIR/clang_64/lib/QtDBus.framework/Versions/5/QtDBus @executable_path/../Frameworks/QtDBus ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change $QTDIR/clang_64/plugins/platforms/libqcocoa.dylib @executable_path/plugins/platforms/libqcocoa.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change $QTDIR/clang_64/plugins/imageformats/libqjpeg.dylib @executable_path/plugins/imageformats/libqjpeg.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /System/Library/Frameworks/AudioToolbox.framework/Versions/A/AudioToolbox @executable_path/libAudioToolbox.dylib ./$APP/$APP.app/Contents/MacOS/$APP

#QtSvg
install_name_tool -id @executable_path/../Frameworks/QtSvg  ./$APP/$APP.app/Contents/Frameworks/QtSvg
install_name_tool -change @rpath/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets ./$APP/$APP.app/Contents/Frameworks/QtSvg
install_name_tool -change @rpath/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui ./$APP/$APP.app/Contents/Frameworks/QtSvg
install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore ./$APP/$APP.app/Contents/Frameworks/QtSvg
install_name_tool -change @rpath/QtDBus.framework/Versions/5/QtDBus @executable_path/../Frameworks/QtDBus ./$APP/$APP.app/Contents/Frameworks/QtSvg

#QtWidgets
install_name_tool -id @executable_path/../Frameworks/QtWidgets  ./$APP/$APP.app/Contents/Frameworks/QtWidgets
install_name_tool -change @rpath/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui ./$APP/$APP.app/Contents/Frameworks/QtWidgets
install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore ./$APP/$APP.app/Contents/Frameworks/QtWidgets
install_name_tool -change @rpath/QtSvg.framework/Versions/5/QtSvg @executable_path/../Frameworks/QtSvg ./$APP/$APP.app/Contents/Frameworks/QtWidgets
install_name_tool -change @rpath/QtDBus.framework/Versions/5/QtDBus @executable_path/../Frameworks/QtDBus ./$APP/$APP.app/Contents/Frameworks/QtWidgets

#QtCore
install_name_tool -id @executable_path/../Frameworks/QtCore  ./$APP/$APP.app/Contents/Frameworks/QtCore
install_name_tool -change @rpath/QtSvg.framework/Versions/5/QtSvg @executable_path/../Frameworks/QtSvg ./$APP/$APP.app/Contents/Frameworks/QtCore
install_name_tool -change @rpath/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui ./$APP/$APP.app/Contents/Frameworks/QtCore
install_name_tool -change @rpath/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets ./$APP/$APP.app/Contents/Frameworks/QtCore
install_name_tool -change @rpath/QtDBus.framework/Versions/5/QtDBus @executable_path/../Frameworks/QtDBus ./$APP/$APP.app/Contents/Frameworks/QtCore

#QtGui
install_name_tool -id @executable_path/../Frameworks/QtGui  ./$APP/$APP.app/Contents/Frameworks/QtGui
install_name_tool -change @rpath/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets ./$APP/$APP.app/Contents/Frameworks/QtGui
install_name_tool -change @rpath/QtSvg.framework/Versions/5/QtSvg @executable_path/../Frameworks/QtSvg ./$APP/$APP.app/Contents/Frameworks/QtGui
install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore ./$APP/$APP.app/Contents/Frameworks/QtGui
install_name_tool -change @rpath/QtDBus.framework/Versions/5/QtDBus @executable_path/../Frameworks/QtDBus ./$APP/$APP.app/Contents/Frameworks/QtGui

#QtPrintSupport
install_name_tool -id @executable_path/../Frameworks/QtPrintSupport  ./$APP/$APP.app/Contents/Frameworks/QtPrintSupport
install_name_tool -change @rpath/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets ./$APP/$APP.app/Contents/Frameworks/QtPrintSupport
install_name_tool -change @rpath/QtSvg.framework/Versions/5/QtSvg @executable_path/../Frameworks/QtSvg ./$APP/$APP.app/Contents/Frameworks/QtPrintSupport
install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore ./$APP/$APP.app/Contents/Frameworks/QtPrintSupport
install_name_tool -change @rpath/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui ./$APP/$APP.app/Contents/Frameworks/QtPrintSupport
install_name_tool -change @rpath/QtDBus.framework/Versions/5/QtDBus @executable_path/../Frameworks/QtDBus ./$APP/$APP.app/Contents/Frameworks/QtPrintSupport

#QtDBus
install_name_tool -id @executable_path/../Frameworks/QtDBus  ./$APP/$APP.app/Contents/Frameworks/QtDBus
install_name_tool -change @rpath/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets ./$APP/$APP.app/Contents/Frameworks/QtDBus
install_name_tool -change @rpath/QtSvg.framework/Versions/5/QtSvg @executable_path/../Frameworks/QtSvg ./$APP/$APP.app/Contents/Frameworks/QtDBus
install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore ./$APP/$APP.app/Contents/Frameworks/QtDBus
install_name_tool -change @rpath/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui ./$APP/$APP.app/Contents/Frameworks/QtDBus

#libqcocoa.dylib
install_name_tool -id @executable_path/plugins/platforms/libqcocoa.dylib  ./$APP/$APP.app/Contents/MacOS/plugins/platforms/libqcocoa.dylib
install_name_tool -change @rpath/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui ./$APP/$APP.app/Contents/MacOS/plugins/platforms/libqcocoa.dylib
install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore ./$APP/$APP.app/Contents/MacOS/plugins/platforms/libqcocoa.dylib
install_name_tool -change @rpath/QtDBus.framework/Versions/5/QtDBus @executable_path/../Frameworks/QtDBus ./$APP/$APP.app/Contents/MacOS/plugins/platforms/libqcocoa.dylib
install_name_tool -change @rpath/QtPrintSupport.framework/Versions/5/QtPrintSupport @executable_path/../Frameworks/QtPrintSupport ./$APP/$APP.app/Contents/MacOS/plugins/platforms/libqcocoa.dylib
install_name_tool -change @rpath/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets ./$APP/$APP.app/Contents/MacOS/plugins/platforms/libqcocoa.dylib
install_name_tool -change @rpath/QtDBus.framework/Versions/5/QtDBus @executable_path/../Frameworks/QtDBus ./$APP/$APP.app/Contents/MacOS/plugins/platforms/libqcocoa.dylib
install_name_tool -change @rpath/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui ./$APP/$APP.app/Contents/MacOS/plugins/imageformats/libqjpeg.dylib
install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore ./$APP/$APP.app/Contents/MacOS/plugins/imageformats/libqjpeg.dylib

#ffmpeg libs
install_name_tool -change /usr/local/lib/libavcodec.57.dylib @executable_path/libavcodec.57.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libavdevice.57.dylib @executable_path/libavdevice.57.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libavfilter.6.dylib @executable_path/libavfilter.6.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libavformat.57.dylib @executable_path/libavformat.57.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libavutil.55.dylib @executable_path/libavutil.55.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libswresample.2.dylib @executable_path/libswresample.2.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libswscale.4.dylib @executable_path/libswscale.4.dylib ./$APP/$APP.app/Contents/MacOS/$APP
$PWD/../$APP/install-scripts/ffmpeg.sh $APP

#libcrypto.1.1.dylib
install_name_tool -id @executable_path/libcrypto.1.1.dylib ./$APP/$APP.app/Contents/MacOS/libcrypto.1.1.dylib
install_name_tool -change /usr/local/lib/libcrypto.1.1.dylib @executable_path/libcrypto.1.1.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libcrypto.1.1.dylib

#libssl.0.9.8.dylib
install_name_tool -id @executable_path/libssl.1.1.dylib ./$APP/$APP.app/Contents/MacOS/libssl.1.1.dylib
install_name_tool -change /usr/local/lib/libssl.1.1.dylib @executable_path/libssl.1.1.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libcrypto.1.1.dylib @executable_path/libcrypto.1.1.dylib ./$APP/$APP.app/Contents/MacOS/libssl.1.1.dylib

#libbz2.1.0.dylib
install_name_tool -id @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/libbz2.1.0.dylib
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/$APP

#libfreetype.6.dylib
install_name_tool -id  @executable_path/libfreetype.6.dylib ./$APP/$APP.app/Contents/MacOS/libfreetype.6.dylib
install_name_tool -change /usr/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/libfreetype.6.dylib
install_name_tool -change /usr/local/lib/libpng16.16.dylib @executable_path/libpng16.16.dylib ./$APP/$APP.app/Contents/MacOS/libfreetype.6.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libfreetype.6.dylib

#libpng16.16.dylib
install_name_tool -id @executable_path/libpng16.16.dylib ./$APP/$APP.app/Contents/MacOS/libpng16.16.dylib
install_name_tool -change /usr/local/lib/libpng16.16.dylib @executable_path/libpng16.16.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libpng16.16.dylib @executable_path/libpng16.16.dylib ./$APP/$APP.app/Contents/MacOS/libpng16.16.dylib
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libpng16.16.dylib

#libz.1.dylib
install_name_tool -id @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libz.1.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libz.1.dylib

#libjpeg.9.dylib
install_name_tool -id @executable_path/libjpeg.9.dylib ./$APP/$APP.app/Contents/MacOS/libjpeg.9.dylib
install_name_tool -change /usr/local/lib/libjpeg.9.dylib @executable_path/libjpeg.9.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change /usr/local/lib/libjpeg.9.dylib @executable_path/libjpeg.9.dylib ./$APP/$APP.app/Contents/MacOS/libjpeg.9.dylib

#libgd.dylib
install_name_tool -id @executable_path/libgd.dylib ./$APP/$APP.app/Contents/MacOS/libgd.dylib
install_name_tool -change $HOME/work/libs/libgd/Bin/libgd.dylib @executable_path/libgd.dylib ./$APP/$APP.app/Contents/MacOS/$APP
install_name_tool -change $HOME/work/libs/libgd/Bin/libgd.dylib @executable_path/libgd.dylib ./$APP/$APP.app/Contents/MacOS/libgd.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libgd.dylib
install_name_tool -change /usr/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib ./$APP/$APP.app/Contents/MacOS/libgd.dylib
install_name_tool -change /usr/local/lib/libpng16.16.dylib @executable_path/libpng16.16.dylib ./$APP/$APP.app/Contents/MacOS/libgd.dylib
install_name_tool -change /usr/local/lib/libjpeg.9.dylib @executable_path/libjpeg.9.dylib ./$APP/$APP.app/Contents/MacOS/libgd.dylib

#libcurl.4.dylib
install_name_tool -id @executable_path/libcurl.4.dylib ./$APP/$APP.app/Contents/MacOS/libcurl.4.dylib
install_name_tool -change /usr/local/lib/libcurl.4.dylib @executable_path/libcurl.4.dylib ./$APP/$APP.app/Contents/MacOS/$APP

#otool -L $APP/$APP.app/Contents/MacOS/$APP
#otool -L $APP/$APP.app/Contents/MacOS/plugins/platforms/libqcocoa.dylib


#make sure the rpaths are correct by running from build dir:
#otool -L ./SquiggleMark/SquiggleMark.app/Contents/MacOS/SquiggleMark
#next run iDMG to create DMG
# drag and drop out SquiggleMark/SpriteMill folder into the iDMG,
# open iDMG inspector, set the name to SquiggleMark/SpriteMill
# close iDMG once it is done, this should create "$APP".dmg
#Mount "$APP".dmg
#From finder, go to out under Devices
# from Menu > View > Hide Toolbar
# type Cmd+J to set background image (SquiggleMark/resources/mac-dmg-sqiugglemark-bg.png)
# unmount "$APP".dmg if it is mounted
#next in the terminal, run this command
#rm -f tmp/SquiggleMark.dmg
#mv SquiggleMark.dmg tmp/SquiggleMark.dmg
#hdiutil convert -format UDZO -o SquiggleMark.dmg tmp/SquiggleMark.dmg
#mv SquiggleMark.dmg SquiggleMark2.5.dmg
#or
#rm -f tmp/SpriteMill.dmg
#mv SpriteMill.dmg tmp/SpriteMill.dmg
#hdiutil convert -format UDZO -o SpriteMill.dmg tmp/SpriteMill.dmg
#mv SpriteMill.dmg SpriteMill1.0.dmg

#mv $HOME/Qt $HOME/Qt~
