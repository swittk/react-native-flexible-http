import * as React from 'react';

import { StyleSheet, View, Text } from 'react-native';
import { multiply, createHTTPServer, HTTPLibServerRequest, HTTPLibServerResponse } from 'react-native-flexible-http';

export default function App() {
  const [result, setResult] = React.useState<number | undefined>();

  React.useEffect(() => {
    multiply(3, 7).then(setResult);
  }, []);
  const cb = React.useCallback((req: HTTPLibServerRequest, res: HTTPLibServerResponse)=>{
    console.log('callback called with headers', res.headers);
    res.set_content('Goodbye worl hasdhjfk;ashjfkdl;as');
    console.log('body is', res.body);
  }, []);

  React.useEffect(()=>{
    console.log('about to create server');
    const server = createHTTPServer();
    console.log('created server');
    server.get('/', cb);
    server.get('/noob.*', (req, res)=>{
      console.log('path is', req.path);
      console.log('params are', req.params);
      res.set_content('Ur a noob');
    });
    server.listen(2500);
    return ()=>{
      server.stop();
    }
  }, []);

  return (
    <View style={styles.container}>
      <Text>Result: {result}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20,
  },
});
