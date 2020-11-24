#!/bin/bash
set -x #echo on

APP=$1
#APP should be one of these two:
#"SquiggleMark"
#"SpriteMill"

#ffmpeg
install_name_tool -id @executable_path/ffmpeg  ./$APP/$APP.app/Contents/MacOS/ffmpeg

install_name_tool -change /usr/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg

install_name_tool -change /usr/local/lib/libavcodec.57.dylib @executable_path/libavcodec.57.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libavdevice.57.dylib @executable_path/libavdevice.57.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libavfilter.6.dylib @executable_path/libavfilter.6.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libavformat.57.dylib @executable_path/libavformat.57.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libavutil.55.dylib @executable_path/libavutil.55.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libswresample.2.dylib @executable_path/libswresample.2.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libswscale.4.dylib @executable_path/libswscale.4.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg

install_name_tool -change /usr/local/lib/libvorbisenc.2.dylib @executable_path/libvorbisenc.2.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libvorbis.0.dylib @executable_path/libvorbis.0.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libtheoraenc.1.dylib @executable_path/libtheoraenc.1.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libtheoradec.1.dylib @executable_path/libtheoradec.1.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libopenh264.2.dylib @executable_path/libopenh264.2.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg
install_name_tool -change /usr/local/lib/libmp3lame.0.dylib @executable_path/libmp3lame.0.dylib ./$APP/$APP.app/Contents/MacOS/ffmpeg

#libavcodec
install_name_tool -id @executable_path/libavcodec.57.dylib  ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libswresample.2.dylib @executable_path/libswresample.2.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libavutil.55.dylib @executable_path/libavutil.55.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libvorbis.0.dylib @executable_path/libvorbis.0.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libvorbisenc.2.dylib @executable_path/libvorbisenc.2.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libtheoraenc.1.dylib @executable_path/libtheoraenc.1.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libtheoradec.1.dylib @executable_path/libtheoradec.1.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libopenh264.2.dylib @executable_path/libopenh264.2.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib
install_name_tool -change /usr/local/lib/libmp3lame.0.dylib @executable_path/libmp3lame.0.dylib ./$APP/$APP.app/Contents/MacOS/libavcodec.57.dylib

#libavdevice
install_name_tool -id @executable_path/libavdevice.57.dylib  ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libavfilter.6.dylib @executable_path/libavfilter.6.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libavfilter.6.dylib @executable_path/libavfilter.6.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libswscale.4.dylib @executable_path/libswscale.4.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libavformat.57.dylib @executable_path/libavformat.57.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libavcodec.57.dylib @executable_path/libavcodec.57.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libswresample.2.dylib @executable_path/libswresample.2.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libavutil.55.dylib @executable_path/libavutil.55.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libvorbisenc.2.dylib @executable_path/libvorbisenc.2.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libvorbis.0.dylib @executable_path/libvorbis.0.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libtheoraenc.1.dylib @executable_path/libtheoraenc.1.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libtheoradec.1.dylib @executable_path/libtheoradec.1.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libopenh264.2.dylib @executable_path/libopenh264.2.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib
install_name_tool -change /usr/local/lib/libmp3lame.0.dylib @executable_path/libmp3lame.0.dylib ./$APP/$APP.app/Contents/MacOS/libavdevice.57.dylib

#libavfilter
install_name_tool -id @executable_path/libavfilter.6.dylib  ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libswscale.4.dylib @executable_path/libswscale.4.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libavformat.57.dylib @executable_path/libavformat.57.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libavcodec.57.dylib @executable_path/libavcodec.57.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libswresample.2.dylib @executable_path/libswresample.2.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libavutil.55.dylib @executable_path/libavutil.55.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libvorbisenc.2.dylib @executable_path/libvorbisenc.2.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libvorbis.0.dylib @executable_path/libvorbis.0.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libtheoraenc.1.dylib @executable_path/libtheoraenc.1.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libtheoradec.1.dylib @executable_path/libtheoradec.1.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libopenh264.2.dylib @executable_path/libopenh264.2.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib
install_name_tool -change /usr/local/lib/libmp3lame.0.dylib @executable_path/libmp3lame.0.dylib ./$APP/$APP.app/Contents/MacOS/libavfilter.6.dylib

#libavformat
install_name_tool -id @executable_path/libavformat.57.dylib  ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libavcodec.57.dylib @executable_path/libavcodec.57.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libswresample.2.dylib @executable_path/libswresample.2.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libavutil.55.dylib @executable_path/libavutil.55.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libvorbisenc.2.dylib @executable_path/libvorbisenc.2.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libvorbis.0.dylib @executable_path/libvorbis.0.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libtheoraenc.1.dylib @executable_path/libtheoraenc.1.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libtheoradec.1.dylib @executable_path/libtheoradec.1.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libopenh264.2.dylib @executable_path/libopenh264.2.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib
install_name_tool -change /usr/local/lib/libmp3lame.0.dylib @executable_path/libmp3lame.0.dylib ./$APP/$APP.app/Contents/MacOS/libavformat.57.dylib

#libavutil
install_name_tool -id @executable_path/libavutil.55.dylib  ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/local/lib/libavutil.55.dylib @executable_path/libavutil.55.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/local/lib/libvorbisenc.2.dylib @executable_path/libvorbisenc.2.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/local/lib/libvorbis.0.dylib @executable_path/libvorbis.0.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/local/lib/libtheoraenc.1.dylib @executable_path/libtheoraenc.1.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/local/lib/libtheoradec.1.dylib @executable_path/libtheoradec.1.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/local/lib/libopenh264.2.dylib @executable_path/libopenh264.2.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib
install_name_tool -change /usr/local/lib/libmp3lame.0.dylib @executable_path/libmp3lame.0.dylib ./$APP/$APP.app/Contents/MacOS/libavutil.55.dylib

#libswscale
install_name_tool -id @executable_path/libswscale.4.dylib  ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libswscale.4.dylib @executable_path/libswscale.4.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libavutil.55.dylib @executable_path/libavutil.55.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libvorbisenc.2.dylib @executable_path/libvorbisenc.2.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libvorbis.0.dylib @executable_path/libvorbis.0.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libtheoraenc.1.dylib @executable_path/libtheoraenc.1.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libtheoradec.1.dylib @executable_path/libtheoradec.1.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libopenh264.2.dylib @executable_path/libopenh264.2.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib
install_name_tool -change /usr/local/lib/libmp3lame.0.dylib @executable_path/libmp3lame.0.dylib ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib

#libswresample
install_name_tool -id @executable_path/libswresample.2.dylib  ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/local/lib/libz.1.dylib @executable_path/libz.1.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/local/lib/libavutil.55.dylib @executable_path/libavutil.55.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/local/lib/libvorbisenc.2.dylib @executable_path/libvorbisenc.2.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/local/lib/libvorbis.0.dylib @executable_path/libvorbis.0.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/local/lib/libtheoraenc.1.dylib @executable_path/libtheoraenc.1.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/local/lib/libtheoradec.1.dylib @executable_path/libtheoradec.1.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/local/lib/libopenh264.2.dylib @executable_path/libopenh264.2.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
install_name_tool -change /usr/local/lib/libmp3lame.0.dylib @executable_path/libmp3lame.0.dylib ./$APP/$APP.app/Contents/MacOS/libswresample.2.dylib
#install_name_tool -change /usr/local/lib/xxx @executable_path/xxx ./$APP/$APP.app/Contents/MacOS/libswscale.4.dylib

#libvorbisenc.2.dylib
install_name_tool -id @executable_path/libvorbisenc.2.dylib  ./$APP/$APP.app/Contents/MacOS/libvorbisenc.2.dylib
install_name_tool -change /usr/local/lib/libvorbis.0.dylib @executable_path/libvorbis.0.dylib ./$APP/$APP.app/Contents/MacOS/libvorbisenc.2.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libvorbisenc.2.dylib

#libtheoraenc.1.dylib
install_name_tool -id @executable_path/libtheoraenc.1.dylib  ./$APP/$APP.app/Contents/MacOS/libtheoraenc.1.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libtheoraenc.1.dylib

#libtheoradec.1.dylib
install_name_tool -id @executable_path/libtheoradec.1.dylib  ./$APP/$APP.app/Contents/MacOS/libtheoradec.1.dylib

#libvorbis.0.dylib
install_name_tool -id @executable_path/libvorbis.0.dylib  ./$APP/$APP.app/Contents/MacOS/libvorbis.0.dylib
install_name_tool -change /usr/local/lib/libogg.0.dylib @executable_path/libogg.0.dylib ./$APP/$APP.app/Contents/MacOS/libvorbis.0.dylib

#libogg.0.dylib
install_name_tool -id @executable_path/libogg.0.dylib  ./$APP/$APP.app/Contents/MacOS/libogg.0.dylib

#libopenh264.2.dylib
install_name_tool -id @executable_path/libopenh264.2.dylib  ./$APP/$APP.app/Contents/MacOS/libopenh264.2.dylib
	
#libmp3lame.0.dylib
install_name_tool -id @executable_path/libmp3lame.0.dylib  ./$APP/$APP.app/Contents/MacOS/libmp3lame.0.dylib



