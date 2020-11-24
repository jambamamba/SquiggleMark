set DST1=C:\work\build-SquiggleMark-Desktop_Qt_5_6_0_MinGW_32bit-Default
;set DST1=C:\work\SquiggleMark-build
set DST2=C:\work\libs\build\win32\bin

set FFMPEG=C:\work\libs\ffmpeg-3.0.2
set LIBOGG=C:\work\libs\libogg-1.3.1
set LIBVORBIS=C:\work\libs\libvorbis-1.3.3
set LIBTHEORA=C:\work\libs\libtheora-1.1.1
set LIBOPENH264=C:\work\libs\openh264
set LAMEMP3=C:\work\libs\lame-3.99.5
set OPENH264=C:\work\libs\\openh264

copy %FFMPEG%\ffmpeg.exe %DST1%
copy %FFMPEG%\ffmpeg.exe %DST2%

copy %FFMPEG%\libavcodec\avcodec-57.dll %DST1%
copy %FFMPEG%\libavcodec\avcodec-57.dll %DST2%

copy %FFMPEG%\libavdevice\avdevice-57.dll %DST1%
copy %FFMPEG%\libavdevice\avdevice-57.dll %DST2%

copy %FFMPEG%\libavfilter\avfilter-6.dll %DST1%
copy %FFMPEG%\libavfilter\avfilter-6.dll %DST2%

copy %FFMPEG%\libavformat\avformat-57.dll %DST1%
copy %FFMPEG%\libavformat\avformat-57.dll %DST2%

copy %FFMPEG%\libavutil\avutil-55.dll %DST1%
copy %FFMPEG%\libavutil\avutil-55.dll %DST2%

copy %FFMPEG%\libswresample\swresample-2.dll %DST1%
copy %FFMPEG%\libswresample\swresample-2.dll %DST2%

copy %FFMPEG%\libswscale\swscale-4.dll %DST1%
copy %FFMPEG%\libswscale\swscale-4.dll %DST2%

copy %LIBOGG%\src\.libs\libogg-0.dll %DST1%
copy %LIBOGG%\src\.libs\libogg-0.dll %DST2%

copy %LIBVORBIS%\lib\.libs\libvorbis-0.dll %DST1%
copy %LIBVORBIS%\lib\.libs\libvorbis-0.dll %DST2%

copy %LIBVORBIS%\lib\.libs\libvorbisenc-2.dll %DST1%
copy %LIBVORBIS%\lib\.libs\libvorbisenc-2.dll %DST2%

copy %LIBTHEORA%\lib\.libs\libtheoradec-1.dll %DST1%
copy %LIBTHEORA%\lib\.libs\libtheoradec-1.dll %DST2%

copy %LIBTHEORA%\lib\.libs\libtheoraenc-1.dll %DST1%
copy %LIBTHEORA%\lib\.libs\libtheoraenc-1.dll %DST2%

copy %LIBOPENH264%\libopenh264.dll %DST1%
copy %LIBOPENH264%\libopenh264.dll %DST2%

copy %LAMEMP3%\libmp3lame\.libs\libmp3lame-0.dll %DST1%
copy %LAMEMP3%\libmp3lame\.libs\libmp3lame-0.dll %DST2%

copy %OPENH264%\libopenh264.dll %DST1%
copy %OPENH264%\libopenh264.dll %DST2%
