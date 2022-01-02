#include <jni.h>
#include <fbjni/fbjni.h>
//#include <jni/JavaScriptExecutorHolder.h>
#include "react-native-flexible-http.h"
#include <CallInvokerHolder.h>
//#include <ReactCommon/CallInvoker.h>

using namespace facebook;

extern "C"
JNIEXPORT jint JNICALL
Java_com_reactnativeflexiblehttp_FlexibleHttpModule_nativeMultiply(JNIEnv *env, jclass type, jint a, jint b) {
    return example::multiply(a, b);
}

jfieldID getPtrFieldId(JNIEnv * env, jobject obj)
{
    static jfieldID ptrFieldId = 0;

    if (!ptrFieldId)
    {
        jclass c = env->GetObjectClass(obj);
        ptrFieldId = env->GetFieldID(c, "ptr", "J");
        env->DeleteLocalRef(c);
    }

    return ptrFieldId;
}

// Inspired by https://ospfranco.com/post/2021/02/24/how-to-create-a-javascript-jsi-module/
// Following the intuition that JNICALL functions always starts with JNIEnv, jclass as first 2 args, followed by our args
// Also heavily adapted from https://github.com/mrousavy/react-native-multithreading/blob/5f9a91b9edb94ab260767831dafa2e55f91af407/android/src/main/cpp/cpp-adapter.cpp#L42
extern "C" JNIEXPORT void JNICALL
Java_com_reactnativeflexiblehttp_FlexibleHttpModule_initialize(JNIEnv *env, jclass clazz,
    jlong jsiRuntimePointer,
    jobject jsCallInvokerHolder // it is of type Lcom/facebook/react/turbomodule/core/CallInvokerHolderImpl
//    jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder
    )
{


    facebook::react::CallInvokerHolder *holder = (facebook::react::CallInvokerHolder *)jsCallInvokerHolder;
    auto jsCallInvoker = holder->getCallInvoker();
    printf("initialize called with runtimePointer %d", jsiRuntimePointer);
    printf("initialize called with callInvoker %d", jsCallInvokerHolder);
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiRuntimePointer);
//    auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
//   jboolean isCopy;
//   const char *docPathString = (env)->GetStringUTFChars(docPath, &isCopy); // This is might not be necessary, but my library moves files in the android file system, so this is just how to pass an android variable to the C++ size

  example::install(*runtime, jsCallInvoker);
}

extern "C" JNIEXPORT void JNICALL
Java_com_reactnativeflexiblehttp_FlexibleHttpModule_cleanup(JNIEnv *env, jclass clazz, jlong jsiPtr)
{
  example::cleanup(*reinterpret_cast<facebook::jsi::Runtime *>(jsiPtr));
}
