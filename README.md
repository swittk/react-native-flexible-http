# react-native-flexible-http

Flexible HTTP server for React Native

Currently fully functioning on iOS.

Android version seems to function now, but is not as extensively used or tested.
- Android version has an error when hot reloading too many times and will crash (due to Android likely not de-initializing the modules completely before starting the next one)

Basically a light wrapper on [cpp-httplib](https://github.com/yhirose/cpp-httplib).

Runs on react-native >= 0.64
- (for those using react-native ~0.64) If you encounter `<Folly/Optional.h>` file not found error while building, then simply comment that line out.


## Installation

```sh
npm install react-native-flexible-http
```

## Usage

```ts
import { createHTTPServer } from "react-native-flexible-http";

// ...

const server = createHTTPServer();

// For get, put, post, and delete methods, add regex matches for paths to handle
server.get('/', (req, res)=>{
    // Set content and content-time
    res.set_content('Hello 2022', 'text/plain');
});
// Regex-matched path
server.get('/prefixedPath.*', (req, res)=>{
    console.log('requested from path', req.path);
    res.set_content('Hello from prefixed path', 'text/plain');
})
server.listen(8000); // Listen on port 8000
```

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT


# Good resources for TurboModules
- https://github.com/barthap/discovering-turbomodules

# Findings (note to self for the future)
- React Native's JSI doesn't have a "dev-friendly" instructions page yet. Most of the stuff I needed to assemble this library are from 2-3 day's worth (of New year 2022 holiday) of Googling around and browsing GitHub issues.
- In JNI, the jobject can simply be casted back to the corresponding registered C++ class
    - See cpp-adapter.cpp (and the corresponding FlexibleHttpModule.java) in `FlexibleHttpModule.initialize`.
        - CallInvokerHolderImpl is passed into cpp-adapter.h as a `jobject`
        - Simply cast from jobject to (CallInvokerHolder *) and use it!
- The calls to initialize and de-initialize modules are as follows in iOS and Android
    - iOS : `-(void)setBridge:(RCTBridge *)bridge` and `-(void)invalidate`
    - Android : `public void initialize()` and `public void onCatalystInstanceDestroy()`
- Since I'm using C++ code, and the callbacks are defined in user Javascript in the JS thread, I need the `CallInvoker` pointer in order to call JS code asynchronously from the server's thread
    - This is obtained from the JS Runtime object
- Overall :: Getting the required properties (JS Runtime and CallInvoker pointer) can be summarized as follows
    - iOS
        - start with our `(RCTBridge *)bridge`
        - Cast to RCTCXXBridge : `RCTCxxBridge *cxxBridge = (RCTCxxBridge *)self.bridge;`
        - Getting Call invoker : `cxxBridge.jsCallInvoker`
        - Getting Runtime pointer : `facebook::jsi::Runtime *runtime = (facebook::jsi::Runtime *)cxxBridge.runtime;`
    - Android
        - start with our `ReactApplicationContext context`
        - Getting Call invoker : `CallInvokerHolderImpl holder = (CallInvokerHolderImpl)context.getCatalystInstance().getJSCallInvokerHolder();`
            - At the C++ adapter side, simply cast the corresponding `jobject` argument at the corresponding position to `CallInvokerHolder *` 
                - Header is `<CallInvokerHolder.h>`
        - Getting Runtime pointer : `JavaScriptContextHolder contextHolder = context.getJavaScriptContextHolder()`
            - Getting the pointer as a `long long` address to pass to the C++ side : `contextHolder.get()`

## Interesting findings / Rant

As of 2022-01-01 (Happy New Year!), Android's Gradle 7.3 build doesn't seem to play nicely with JDK 17 and refuses to build the React-Native project, throwing out this meaningless error.
````
> Task :app:processDebugMainManifest FAILED

FAILURE: Build failed with an exception.

* What went wrong:
Execution failed for task ':app:processDebugMainManifest'.
> Unable to make field private final java.lang.String java.io.File.path accessible: module java.base does not "opens java.io" to unnamed module @d2b700f
````
After searching for some solutions on the internet, it seemed as if JDK 16 was just supported recently in Gradle 7.x. Suspecting incompatibility with JDK 17, I tried calling ``export JAVA_HOME=`/usr/libexec/java_home -v 16` ``
then lo and behold, `./gradlew build clean` executed fine without errors.

Coming from development using primarily only C and C-like languages, I never expected such a fragile build system to be used this widely. Maybe that's just Java being Java, idk.


- build.gradle configuration
    - (No longer using prefabs since it conflicts with other stuff) ~~~needs `prefab true` in order to get fbjni imports (see https://github.com/facebookincubator/fbjni/blob/main/docs/android_setup.md)~~~

- 02/01/2021 : now building, but seems need to fix some bugs with jni stuff (jobject is an invalid JNI transition frame reference or invalid reference)
- 02/01/2021 17:00 : NOW WORKING
    - Android example now builds and runs (only working via Android studio though; react-native run android complains about cmake, etc)
    - Native module now seems to load and called just fine
    - Getting this error on Android when reloading : `Catalyst Instance has already disappeared: requested by WebSocketModule`
        - Leads to crashes after a few reloads
        - Issue similar to the one seen here : https://github.com/facebook/react-native/issues/28992
            - They say it is due to react-native's hot reloading creating new instances of the native module before disposing of the old ones