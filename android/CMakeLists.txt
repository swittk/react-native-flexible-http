cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 17)

set (BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set (PACKAGE_NAME "reactnativeflexiblehttp")

# find fbjni package
find_package(fbjni REQUIRED CONFIG)

# If this doesn't work maybe follow modifications from https://ospfranco.com/post/2021/02/24/how-to-create-a-javascript-jsi-module/

# Specifies a path to native header files.
# This was inspired by modifications from https://ospfranco.com/post/2021/02/24/how-to-create-a-javascript-jsi-module/
# but actually more like https://github.com/barthap/discovering-turbomodules/blob/main/android/CMakeLists.txt
include_directories(
        ../cpp
        ../node_modules/react-native/React
        ../node_modules/react-native/React/Base
        ../node_modules/react-native/ReactCommon
        ../node_modules/react-native/ReactCommon/jsi
        ../node_modules/react-native/ReactCommon/callinvoker
        ../node_modules/react-native/ReactAndroid/src/main/java/com/facebook/react/turbomodule/core/jni/ReactCommon
        # ../node_modules/react-native/ReactAndroid/src/main/jni/react/jni/JavaScriptExecutorHolder.h
        # ../node_modules/react-native/ReactAndroid/src/main/jni/react/jni
        ../node_modules/react-native/ReactAndroid/src/main/jni/react
        ../node_modules/react-native/ReactCommon/turbomodule/core
        ../node_modules/react-native/ReactCommon/turbomodule
        ../node_modules/react-native/React
        ../node_modules/react-native/React/Base
        ../node_modules/react-native/ReactCommon/jsi
)


add_library(
    ${PACKAGE_NAME}
            SHARED
            ../node_modules/react-native/ReactCommon/jsi/jsi/jsi.cpp
            ../node_modules/react-native/ReactAndroid/src/main/java/com/facebook/react/turbomodule/core/jni/ReactCommon/CallInvokerHolder.cpp
            # ../node_modules/react-native/ReactCommon/turbomodule/core/TurboModule.cpp
            ../cpp/react-native-flexible-http.cpp
            cpp-adapter.cpp
)

# link fbjni and logger to package
target_link_libraries(${PACKAGE_NAME} fbjni::fbjni android log)

# file (GLOB LIBFBJNI_INCLUDE_DIR "${BUILD_DIR}/fbjni-*-headers.jar/")

# find libraries

# file (GLOB LIBRN_DIR "${BUILD_DIR}/react-native-0*/jni/${ANDROID_ABI}")
# file (GLOB LIBJSC_DIR "${BUILD_DIR}/android-jsc*.aar/jni/${ANDROID_ABI}")
# file (GLOB LIBREANIMATED_DIR "${BUILD_DIR}/react-native-reanimated-*.aar/jni/${ANDROID_ABI}")


# find_library(
#         LOG_LIB
#         log
# )
# find_library(
#         FBJNI_LIB
#         fbjni
#         PATHS ${LIBRN_DIR}
#         NO_CMAKE_FIND_ROOT_PATH
# )

# find_library(
#         REACT_NATIVE_JNI_LIB
#         reactnativejni
#         PATHS ${LIBRN_DIR}
#         NO_CMAKE_FIND_ROOT_PATH
# )

# find_library(
#         REACT_NATIVE_UTILS_LIB
#         reactnativeutilsjni
#         PATHS ${LIBRN_DIR}
#         NO_CMAKE_FIND_ROOT_PATH
# )

# # linking

# find_library(
#         FBJNI_LIBRARY fbjni
#         PATHS ${libfbjni_link_DIRS}
#         NO_CMAKE_FIND_ROOT_PATH
# )
