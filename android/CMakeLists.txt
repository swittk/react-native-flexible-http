cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 17)
set (NODE_MODULES_DIR "../node_modules")
set (RN_SO_DIR ${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/jni/first-party/react/jni)
set (FBJNI_HEADERS_DIR ${RN_SO_DIR}/../../fbjni/headers)
# added a bunch of code from reanimated repo
#if(${REACT_NATIVE_TARGET_VERSION} LESS 66)
  set (
          INCLUDE_JSI_CPP
          "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi/jsi/jsi.cpp"
  )
#  set (
#          INCLUDE_JSIDYNAMIC_CPP
#          "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi/jsi/JSIDynamic.cpp"
#  )
#endif()

set (BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set (PACKAGE_NAME "reactnativeflexiblehttp")

#file (GLOB LIBFBJNI_INCLUDE_DIR ${FBJNI_HEADERS_DIR})

# find fbjni package
find_package(fbjni REQUIRED CONFIG)

# If this doesn't work maybe follow modifications from https://ospfranco.com/post/2021/02/24/how-to-create-a-javascript-jsi-module/

# Specifies a path to native header files.
# This was inspired by modifications from https://ospfranco.com/post/2021/02/24/how-to-create-a-javascript-jsi-module/
# but actually more like https://github.com/barthap/discovering-turbomodules/blob/main/android/CMakeLists.txt
include_directories(
        ../cpp
        "${FBJNI_HEADERS_DIR}"
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
            ${INCLUDE_JSI_CPP}
            ${INCLUDE_JSIDYNAMIC_CPP}
#           ../node_modules/react-native/ReactCommon/jsi/jsi/jsi.cpp
            ../node_modules/react-native/ReactAndroid/src/main/java/com/facebook/react/turbomodule/core/jni/ReactCommon/CallInvokerHolder.cpp
            # ../node_modules/react-native/ReactCommon/turbomodule/core/TurboModule.cpp
            ../cpp/react-native-flexible-http.cpp
            cpp-adapter.cpp
)

#add_library(
#        turbomodulejsijni
#        # Sets the library as a shared library.
#        SHARED
#)

# link fbjni and logger to package
target_link_libraries(${PACKAGE_NAME} fbjni::fbjni android log)
