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
