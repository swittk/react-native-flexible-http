package com.reactnativeflexiblehttp;

import android.util.Log;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.LifecycleEventListener;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.soloader.SoLoader;

// In initialization, get CallInvoker for Android (https://github.com/react-native-community/discussions-and-proposals/issues/91#issuecomment-973847136)
// This site here helps a lot with JSI initialization for me who knows little about Android-specific stuff
// also inspired by here https://github.com/SudoPlz/sp-react-native-mqtt/blob/master/android/src/main/java/com/tuanpm/RCTMqtt/RCTMqttModule.java#L134:L137
// Partially based on https://blog.notesnook.com/getting-started-react-native-jsi/
@ReactModule(name = FlexibleHttpModule.NAME)
public class FlexibleHttpModule extends ReactContextBaseJavaModule {
    public static final String NAME = "FlexibleHttp";
    private final ReactApplicationContext reactContext;

    public FlexibleHttpModule(ReactApplicationContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;
//        Log.d("SwittFlexibleHTTP", "Context is " + reactContext);
//        ReactApplicationContext myContext = this.getReactApplicationContext();
//        Log.d("SwittFlexibleHTTP", "MyContext is " + myContext);
////        this.initLib();
//      // Need to run this after context finished loading otherwise shit will break
//      new java.util.Timer().schedule(
//        new java.util.TimerTask() {
//          @Override
//          public void run() {
//            // your code here
//            Log.e("SwittRetry", "about to start in 5000 ms");
//            initLib();
//          }
//        },
//        5000
//      );
    }

      void initLib() {
        ReactApplicationContext context = this.reactContext;
        Log.d("Switt", "App info" + context.getApplicationInfo());
      try {
        // Currently crashing here
      /*
      Exception in native call
      java.lang.NullPointerException: Attempt to invoke interface method 'com.facebook.react.bridge.JavaScriptContextHolder com.facebook.react.bridge.CatalystInstance.getJavaScriptContextHolder()' on a null object reference
       */
        JavaScriptContextHolder jsContext = context.getJavaScriptContextHolder();
        this.installLib(this.reactContext, jsContext);
      } catch(Error e) {
        new java.util.Timer().schedule(
          new java.util.TimerTask() {
            @Override
            public void run() {
              // your code here
              Log.e("SwittRetry", "about to retry in 5000 ms, error "+ e);
              initLib();
            }
          },
          5000
        );
      }
    }

    @Override
    @NonNull
    public String getName() {
        return NAME;
    }

    // These lines are inspired by https://github.com/mrousavy/react-native-multithreading/blob/5f9a91b9edb94ab260767831dafa2e55f91af407/android/src/main/java/com/reactnativemultithreading/MultithreadingModule.java#L42
    // Adapted to fit implementation inspired by https://ospfranco.com/post/2021/02/24/how-to-create-a-javascript-jsi-module/
    private static native void initialize(long jsiRuntimePointer,
                                           CallInvokerHolderImpl jsCallInvokerHolder);
    private static native void cleanup(long jsiRuntimePointer);

    // This is the method that will need to be called at module startup in order to initialize stuff
    public void installLib(ReactApplicationContext context, JavaScriptContextHolder jsContext) {
      Log.d("Switt", "Installing lib");
      Log.d("Switt", "Context is" + context);
      Log.d("Switt", "jsContext is" + jsContext);
      // Get jsCallInvoker https://github.com/react-native-community/discussions-and-proposals/issues/40#issuecomment-858375750
      CallInvokerHolderImpl holder = (CallInvokerHolderImpl) context.getCatalystInstance().getJSCallInvokerHolder();
      FlexibleHttpModule.initialize(jsContext.get(), holder);
      Log.d("Switt", "Initialized lib");
    }

    // This method is called automatically (defined in BaseJavaModule.java)
    // "called on the appropriate method when a life cycle event occurs.
    @Override
    public void initialize() {
      this.initLib();
    }

    // This method is called automatically (defined in BaseJavaModule.java)
    // "called on the appropriate method when a life cycle event occurs.
    // This method is equivalent to Objective-C's 'invalidate'
    @Override
    public void onCatalystInstanceDestroy() {
        FlexibleHttpModule.cleanup(this.reactContext.getJavaScriptContextHolder().get());
        // FlexibleHttpModule.cleanup(this.getReactApplicationContext());
    }


    static {
        try {
            // Used to load the 'native-lib' library on application startup.
            System.loadLibrary("reactnativeflexiblehttp");
        } catch (Exception ignored) {
        }
    }

    // Example method
    // See https://reactnative.dev/docs/native-modules-android
    @ReactMethod
    public void multiply(int a, int b, Promise promise) {
        promise.resolve(nativeMultiply(a, b));
    }

    public static native int nativeMultiply(int a, int b);
}
