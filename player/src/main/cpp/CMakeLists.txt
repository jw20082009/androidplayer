# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS " -frtti -std=gnu++11  -D_CGE_USE_FFMPEG_ -DDEBUG -DANDROID_NDK -DCGE_TEXTURE_PREMULTIPLIED=1 -D__STDC_CONSTANT_MACROS -D_CGE_DISABLE_GLOBALCONTEXT_ -O3 -ffast-math -D_CGE_ONLY_FILTERS_")
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
set(CGE_ROOT ../)
set(lib_DIR ../../../../src/main/libs/${ANDROID_ABI})

file(GLOB include_headers
        ${CGE_ROOT}/ffmpeg
        include/*.h)

include_directories(
        ${include_headers}
)

file(GLOB native_srcs
        native-lib.cpp
        cgeVideoDecoder.cpp
        svVideoPlayer.cpp
        cgeGlobal.cpp
        cgeCommonDefine.cpp)

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${native_srcs}
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)
find_library(egl-lib
        EGL
        )
find_library(glesv2-lib
        GLESv2
        )
find_library(graphics-lib
        jnigraphics
        )
find_library(atomiclib
        atomic
        )
# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
add_library(libffmpeg SHARED IMPORTED)
set_target_properties(libffmpeg
        PROPERTIES IMPORTED_LOCATION
        ${lib_DIR}/libffmpeg.so)

target_link_libraries( # Specifies the target library.
        native-lib
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        libffmpeg
        ${egl-lib}
        ${glesv2-lib}
        ${graphics-lib}
        ${atomic})