
    cmake_minimum_required(VERSION 3.6.0)

    include_directories(src/main/cpp/include)
    include_directories(src/main/cpp/soundtouch/include)
    include_directories(src/main/cpp/soundtouch/sources)

    add_library( avcodec-57 SHARED IMPORTED)
    set_target_properties( avcodec-57
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavcodec-57.so)

    add_library( avdevice-57 SHARED IMPORTED)
    set_target_properties( avdevice-57
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavdevice-57.so)

    add_library( avfilter-6 SHARED IMPORTED)
    set_target_properties( avfilter-6
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavfilter-6.so)

    add_library( avformat-57 SHARED IMPORTED)
    set_target_properties( avformat-57
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavformat-57.so)

    add_library( avutil-55 SHARED IMPORTED)
    set_target_properties( avutil-55
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavutil-55.so)

    add_library( postproc-54 SHARED IMPORTED)
    set_target_properties( postproc-54
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libpostproc-54.so)

    add_library( swresample-2 SHARED IMPORTED)
    set_target_properties( swresample-2
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswresample-2.so)

    add_library( swscale-4 SHARED IMPORTED)
    set_target_properties( swscale-4
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswscale-4.so)

    add_library(
            AudioPlayer
            SHARED
            src/main/cpp/audio/AudioPlayer.cpp
            src/main/cpp/audio/PlayStatus.cpp
            src/main/cpp/audio/Audio.cpp
            src/main/cpp/audio/AVPacketBlockQueue.cpp
            src/main/cpp/audio/AudioDecoder.cpp
            src/main/cpp/audio/AudioOutput.cpp
            src/main/cpp/audio/AudioPlayerController.cpp
            src/main/cpp/audio/JavaCaller.cpp

            src/main/cpp/soundtouch/sources/AAFilter.cpp
            src/main/cpp/soundtouch/sources/FIFOSampleBuffer.cpp
            src/main/cpp/soundtouch/sources/FIRFilter.cpp
            src/main/cpp/soundtouch/sources/cpu_detect_x86.cpp
            src/main/cpp/soundtouch/sources/sse_optimized.cpp
            src/main/cpp/soundtouch/sources/RateTransposer.cpp
            src/main/cpp/soundtouch/sources/SoundTouch.cpp
            src/main/cpp/soundtouch/sources/InterpolateCubic.cpp
            src/main/cpp/soundtouch/sources/InterpolateLinear.cpp
            src/main/cpp/soundtouch/sources/InterpolateShannon.cpp
            src/main/cpp/soundtouch/sources/TDStretch.cpp
            src/main/cpp/soundtouch/sources/PeakFinder.cpp
    )

    target_link_libraries(
            AudioPlayer

            avcodec-57
            avdevice-57
            avfilter-6
            avformat-57
            avutil-55
            postproc-54
            swresample-2
            swscale-4

            OpenSLES
            log
    )



