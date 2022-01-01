# react-native-flexible-http

Flexible HTTP server for React Native

Currently fully functioning on iOS.

Android version will come soon as soon as I figure how to set up Android Studio properly on Apple Silicon dev machines.

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
    - needs `prefab true` in order to get fbjni imports (see https://github.com/facebookincubator/fbjni/blob/main/docs/android_setup.md)
