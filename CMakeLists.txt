#to build on MacOS from command line
#mkdir ../SquiggleMark-build
#/Applications/CMake.app/Contents/bin/ccmake ../SquiggleMark
# CMAKE_BUILD_TYPE Release
# CMAKE_OSX_ARCHITECTURES x86_64
# CMAKE_OSX_DEPLOYMENT_TARGET 10.10
# CMAKE_OSX_SYSROOT 
# sudo mv ~/Downloads/MacOSX10.10.sdk /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs
# /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk
#( you can get SDKs from https://github.com/phracker/MacOSX-SDKs/releases, download and extract to folder above. 
# extract using cmd: gunzip MacOSX10.10.sdk.tar.xz )
# cp MacOSX10.10.sdk /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/
# Qt5Concurrent_Dir /Users/OsleTek/Qt5/5.5/clang_64/lib/cmake/Qt5Concurrent/
# Qt5Core_DIR /Users/OsleTek/Qt5/5.5/clang_64/lib/cmake/Qt5Core/
# Qt5Gui_DIR /Users/OsleTek/Qt5/5.5/clang_64/lib/cmake/Qt5Gui
# Qt5Svg_DIR /Users/OsleTek/Qt5/5.5/clang_64/lib/cmake/Qt5Svg
# Qt5Widgets_DIR /Users/OsleTek/Qt5/5.5/clang_64/lib/cmake/Qt5Widgets
# press c to configure and g to generate, do not run cmake after you exit ccmake!
# make -j32
# then for mac, run ../SquiggleMark/install-scripts/macinstall.sh
# to run
# ./SquiggleMark.app/Contents/MacOS/SquiggleMark
# to debug, add this in the Run Environment variables
# XXX do not do this XXX DYLD_LIBRARY_PATH = ${HOME}/work/libs/build/osx/bin

#To build on Ubuntu
#cd buildDir
#ccmake ..
#LINUX
# Qt5Concurrent_DIR                /home/dev/oosman/Qt/5.15.0/gcc_64/lib/cmake/Qt5Concurrent        
# Qt5Core_DIR                      /home/dev/oosman/Qt/5.15.0/gcc_64/lib/cmake/Qt5Core
# Qt5Gui_DIR                       /home/dev/oosman/Qt/5.15.0/gcc_64/lib/cmake/Qt5Gui
# Qt5MultimediaWidgets_DIR         /home/dev/oosman/Qt/5.15.0/gcc_64/lib/cmake/Qt5MultimediaWidgets 
# Qt5Multimedia_DIR                /home/dev/oosman/Qt/5.15.0/gcc_64/lib/cmake/Qt5Multimedia        
# Qt5Network_DIR                   /home/dev/oosman/Qt/5.15.0/gcc_64/lib/cmake/Qt5Network           
# Qt5Svg_DIR                       /home/dev/oosman/Qt/5.15.0/gcc_64/lib/cmake/Qt5Svg
# Qt5Widgets_DIR                   /home/dev/oosman/Qt/5.15.0/gcc_64/lib/cmake/Qt5Widgets           
# Qt5X11Extras_DIR                 /home/dev/oosman/Qt/5.15.0/gcc_64/lib/cmake/Qt5X11Extras# CMAKE_OSX_SYSROOT 
#To run project
#Add to LD_LIBRARY_PATH:
#:/home/dev/oosman/work.git/Madusa/build/lib
# after compiling code, to run application, in build directory:
#ln -s /home/dev/$DOCKERUSER/Qt/5.15.0/gcc_64/plugins/platforms platforms


cmake_minimum_required(VERSION 3.11)
set(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} " ) # add -v for verbose mode, then add -verbose in collect2 command for more verbose output
SET(CMAKE_CXX_STANDARD 14)
set(LIBS_DIR "$ENV{HOME}/$ENV{DOCKERUSER}/.leila/lib")
set(QT_DIR "$ENV{HOME}/$ENV{DOCKERUSER}/Qt/5.15.0")

#rm -fr ../SquiggleMark-build/*
#rm -fr ../SquiggleMark-build/CMakeCache.txt

SET(PROJECT_NAME SquiggleMark)
PROJECT(${PROJECT_NAME})
#or
#SET(PROJECT_NAME SpriteMill)

#SET(HOME_DIR "/Users/oosman")
SET(HOME_DIR "/Users/OsleTek")

SET(CMAKE_PREFIX_PATH
#    ${HOME_DIR}/Qt/5.5/gcc_64/lib/cmake
#    ${HOME_DIR}/Qt5/5.5/clang_64/lib/cmake
	/mingw64/bin/cmake.exe
)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(_MACHINE_ARCH_FLAG "/machine:x86")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -emit-llvm -S")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")
#add_compile_options(-std=c++11)
#add_compile_op_tions(-stdlib=libstdc++)
#add_compile_options(-stdlib=libc++)

FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Concurrent REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Svg REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${Qt5Widgets_INCLUDES}
#    ../libs/curl-7.32.0/include
    ${LIBS_DIR}/zlib-1.2.11
    ${LIBS_DIR}/libpng-1.6.37
    ${LIBS_DIR}/jpegsr9c
    ${LIBS_DIR}/openssl/include
    ${LIBS_DIR}/alsa/include
    ${LIBS_DIR}/gif/lib
    ${LIBS_DIR}/ffmpeg
    ${LIBS_DIR}/freetype-2.10.0/include
    ../License
    ../Updater
    )

# this will run uic on .ui files:
QT5_WRAP_UI(PROJECT_UI_HDRS
    MainWindow.ui
    ControlsAreaWidget.ui
    FileListLoading.ui
    FileSelectionWidget.ui
    ../License/LicenseWidget.ui
    MoviePlayerWidget.ui
    PreviewAreaWidget.ui
    ReferenceImageSizeHelp.ui
    RotationWidget.ui
    QualitySlider.ui
    ../Updater/UpdaterWidget.ui
)
#sometimes you have to manually run the uic
#C:\qt5\5.5\mingw492_32\bin\uic.exe mainwindow.ui > ui_MainWindow.h
#/mingw64/bin/uic.exe ../SquiggleMark/ControlsAreaWidget.ui > ui_ControlsAreaWidget.h
#~/Qt5/5.5/clang_64/bin/uic MainWindow.ui > ui_MainWindow.h
# this will run moc:
#QT4_WRAP_CPP(PROJECT_MOC_SRCS
#    MainWindow.h
#    WaterMarkContainer.h
#    WatermarkContainerBgWidget.h
#    FileListView.h)
# generate rules for building source files from the resources
QT5_ADD_RESOURCES(PROJECT_RESOURCES
    resources.qrc
)
ADD_DEFINITIONS(
    ${Qt5Widgets_DEFINITIONS}
    ${Qt5Concurrent_DEFINITIONS}
    -D__STDC_FORMAT_MACROS
    -DWin64
    -DFT_CONFIG_OPTION_SUBPIXEL_RENDERING
    -DFT2_BUILD_LIBRARY
    -DHAVE_FT2BUILD_H
    -DHAVE_LIBFREETYPE
    -DHAVE_LIBJPEG
    -DHAVE_LIBPNG
    -DHAVE_LIBZ
    -DHAVE_STDINT_H
    -DNONDLL
#    -DHAVE_LIBWEBP

)

SET(PROJECT_HEADERS
    Crypto.h
    MainSplitter.h
    QualitySlider.h
)

SET(PROJECT_SOURCES
    AudioPlayer.h
    AudioPlayer.cpp
    ControlsAreaWidget.cpp
    Crypto.cpp
    DeviceTableView.cpp
    FFMpeg.cpp
    ffplay3.c
    FileListLoading.cpp
    FileListView.cpp
    FileSelectionWidget.cpp
    FontHelper.cpp
    ImageHelper.cpp
    IosDeviceIcons.cpp
    IosDeviceImages.cpp
    ITunesScreenShots.cpp
    ../License/NetworkRequest.h
    ../License/NetworkRequest.cpp
    ../License/License.h
    ../License/License.cpp
    ../License/LicenseComm.h
    ../License/LicenseComm.cpp
    ../License/LicenseWidget.h
    ../License/LicenseWidget.cpp
    MacOSAppIcons.cpp
    main.cpp
    MainSplitter.cpp
    MainWindow.cpp
    ModalDialog.h
    ModalDialog.cpp
    MoviePlayerWidget.cpp
    PreviewArea.cpp
    PreviewAreaWidget.cpp
    QualitySlider.cpp
    ReferenceImageSizeHelp.cpp
    RotationWidget.cpp
    SupportedFileTypes.cpp
    ../Updater/UpdaterWidget.h
    ../Updater/UpdaterWidget.cpp
    VideoPlayer.cpp
    WaterMark.cpp
    cmdutils.c
    cmdutils.h
    resources/squigglemark.rc
)

SET(SSLLibs "")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    LINK_DIRECTORIES(
        /usr/lib
        ${LIBS_DIR}/ffmpeg/libavcodec/
        ${LIBS_DIR}/ffmpeg/libavfilter/
        ${LIBS_DIR}/ffmpeg/libavresample/
        ${LIBS_DIR}/ffmpeg/libpostproc/
        ${LIBS_DIR}/ffmpeg/libswscale/
        ${LIBS_DIR}/ffmpeg/libavdevice/
        ${LIBS_DIR}/ffmpeg/libavformat/
        ${LIBS_DIR}/ffmpeg/libavutil/
        ${LIBS_DIR}/ffmpeg/libswresample/
        ${LIBS_DIR}/gif/lib/.libs/
        ${LIBS_DIR}/alsa/src/.libs/
    )
#    SET(CurlLib
#            curl
#        )
    SET(SSLLibs
        crypto
        ssl
        dl
    )
    SET(GDLib
        gd
        )
    SET(FreetypeLib
        freetype
        )
     SET(FFMpegLibs
        avcodec
        avdevice
        avformat
        avfilter
        avutil
        swresample
        swscale
    )
    SET(AudioLib
    )
    SET(PROJECT_SOURCES
        ${PROJECT_SOURCES}
        PcmPlayerLinux.cpp
    )
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    INCLUDE_DIRECTORIES(
        /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/System/Library/Frameworks/AudioToolbox.framework/Versions/A/Headers/
    )
    LINK_DIRECTORIES(
        ${HOME_DIR}/work/libs/build/osx/bin
    )
#    SET(CurlLib
#            curl
#        )
    SET(SSLLibs
        crypto
        ssl
        dl
    )
     SET(FFMpegLibs
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
    )
    SET(ZLib
            z
        )
    SET(JPEGLib
        jpeg.9
        )
    SET(PNGLib
        png16
        )
    SET(GDLib
        gd
        )
    SET(FreetypeLib
        freetype
        )
    SET(AudioLib
        AudioToolbox
    )
    SET(PROJECT_SOURCES
        ${PROJECT_SOURCES}
        PcmPlayerOsx.cpp
    )
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        SET(PLATFORM MACOSX_BUNDLE)
        SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES MACOSX_BUNDLE TRUE)
        SET(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT})
        SET(MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME}.ico)
        SET(App_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/${PROJECT_NAME}.ico)
        SET_SOURCE_FILES_PROPERTIES(${App_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endif (CMAKE_BUILD_TYPE STREQUAL "Release")
    
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    LINK_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}/../libs/build/win64
    )
    SET(SSLLibs
        ssl-1_1-x64
        crypto-1_1-x64
    )
     SET(FFMpegLibs
        avcodec-58
        avdevice-58
        avfilter-7
        avformat-58
        avutil-56
        swresample-3
        swscale-5
    )
    SET(ZLib
            z
        )
    SET(JPEGLib
        libjpeg-9
        )
    SET(PNGLib
        png16-16
        )
    SET(GDLib
        libgd
        )
    SET(FreetypeLib
        libfreetype-6
        )
    SET(AudioLib
        winmm
    )
    SET(PROJECT_SOURCES
        ${PROJECT_SOURCES}
        PcmPlayerWin.cpp
    )
    SET(PLATFORM WIN32)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

ADD_EXECUTABLE(${PROJECT_NAME} ${PLATFORM}
    ${App_ICON}
    ${PROJECT_HEADERS}
    ${PROJECT_SOURCES}
    ${PROJECT_MOC_SRCS}
    ${PROJECT_UI_HDRS}
    ${PROJECT_RESOURCES}
)

#MESSAGE(STATUS "Baah Hum Bug!")

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${Qt5Widgets_LIBRARIES}
    Qt5::Network
    ${Qt5Svg_LIBRARIES}
    ${SSLLibs}
    ${ZLib}
    ${JPEGLib}
    ${PNGLib}
    ${GDLib}
    ${FreetypeLib}
    ${FFMpegLibs}
    ${AudioLib}
)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS -LC:/work/libs/build/win64)
        SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS -Wl,-subsystem,windows)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS -L/usr/local/bin/squigglemark/)
    execute_process(COMMAND bash "-c" "/bin/ln -sf ${QT_DIR}/gcc_64/plugins/platforms platforms"
    OUTPUT_VARIABLE LN_CMD_RES
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

