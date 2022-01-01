package com.reactnativeflexiblehttp;

import androidx.annotation.NonNull;

import com.facebook.react.ReactPackage;
import com.facebook.react.bridge.JSIModulePackage;
import com.facebook.react.bridge.JSIModuleSpec;
import com.facebook.react.bridge.JavaScriptContextHolder;

import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.uimanager.ViewManager;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class FlexibleHttpPackage implements ReactPackage {
    @NonNull
    @Override
    public List<NativeModule> createNativeModules(@NonNull ReactApplicationContext reactContext) {
        List<NativeModule> modules = new ArrayList<>();
        modules.add(new FlexibleHttpModule(reactContext));
        return modules;
    }

//    @NonNull
//    @Override
//    /**@return a {@link List< JSIModuleSpec >} that contain the list of JSI Modules. */
//    public List<JSIModuleSpec> getJSIModules(
//    ReactApplicationContext reactApplicationContext, JavaScriptContextHolder jsContext) {
//      try {
//        return super.getJSIModules(reactApplicationContext, jsContext);
//      } finally {
//        FlexibleHttpModule.install(reactApplicationContext, jsContext);
//      }
//    }


  @NonNull
    @Override
    public List<ViewManager> createViewManagers(@NonNull ReactApplicationContext reactContext) {
        return Collections.emptyList();
    }
}
