#!/bin/bash
#

BIN_DIR=`pwd`
ICON_NAME=peacock-256
TMP_DIR=`mktemp --directory`
DESKTOP_FILE=$TMP_DIR/OsLeTek-SquiggleMark.desktop
cat << EOF > $DESKTOP_FILE
[Desktop Entry]
Version=1.0
Encoding=UTF-8
Name=SquiggleMark
Keywords=
GenericName=SquiggleMark - The Watermarking App
Type=Application
Categories=Development;RevisionControl
Terminal=false
StartupNotify=true
Exec="$BIN_DIR/run-squigglemark" %u
MimeType=x-scheme-handler/SquiggleMark
Icon=$ICON_NAME.png
EOF

xdg-desktop-menu install $DESKTOP_FILE
xdg-icon-resource install --size  32 "$BIN_DIR/resources/peacock-32.png"  $ICON_NAME
xdg-icon-resource install --size  48 "$BIN_DIR/resources/peacock-48.png"  $ICON_NAME
xdg-icon-resource install --size  64 "$BIN_DIR/resources/peacock-64.png"  $ICON_NAME
xdg-icon-resource install --size 128 "$BIN_DIR/resources/peacock-128.png" $ICON_NAME
xdg-icon-resource install --size 256 "$BIN_DIR/resources/peacock-256.png" $ICON_NAME

rm $DESKTOP_FILE
rm -R $TMP_DIR
