#cd /Users/oosman/Desktop/work/SquiggleMark-build    
#otool -l SquiggleMark | grep lib
#         name /usr/lib/dyld (offset 12)
#         name /Users/oosman/Qt/5.3/clang_64/lib/QtWidgets.framework/Versions/5/QtWidgets (offset 24)
#         name /Users/oosman/Qt/5.3/clang_64/lib/QtSvg.framework/Versions/5/QtSvg (offset 24)
#         name /usr/lib/libcrypto.0.9.8.dylib (offset 24)
#         name /usr/lib/libssl.0.9.8.dylib (offset 24)
#         name /usr/lib/libSystem.B.dylib (offset 24)
#         name /Users/oosman/Qt/5.3/clang_64/lib/QtGui.framework/Versions/5/QtGui (offset 24)
#         name /Users/oosman/Qt/5.3/clang_64/lib/QtCore.framework/Versions/5/QtCore (offset 24)
#         name /usr/lib/libc++.1.dylib (offset 24)
# tools:
# export DYLD_PRINT_LIBRARIES=1
# unset DYLD_PRINT_LIBRARIES
# otool -l <app>
cat /dev/null > macinstall.sh
echo "mv /Users/oosman/Qt~ /Users/oosman/Qt" >> macinstall.sh
echo "rm -fr SquiggleMark.app" >> macinstall.sh
echo "mkdir -p SquiggleMark.app/Contents/Frameworks" >> macinstall.sh
echo "mkdir -p SquiggleMark.app/Contents/MacOS/platforms" >> macinstall.sh
echo "cp SquiggleMark SquiggleMark.app/Contents/MacOS" >> macinstall.sh
echo "chmod +x SquiggleMark.app/Contents/MacOS/SquiggleMark" >> macinstall.sh
otool -l SquiggleMark | awk '/framework/ {print "cp "$2" SquiggleMark.app/Contents/Frameworks"}' >> macinstall.sh
otool -l SquiggleMark | awk  '/framework/ {print $2}' | \
awk -F'/' '{print "install_name_tool -id @executable_path/../Frameworks/"$NF"  SquiggleMark.app/Contents/Frameworks/"$NF} \
 {print "install_name_tool -change "$0" @executable_path/../Frameworks/"$NF" SquiggleMark.app/Contents/MacOS/SquiggleMark"}' \
>> macinstall.sh
echo "mv /Users/oosman/Qt /Users/oosman/Qt~" >> macinstall.sh
chmod +x macinstall.sh

