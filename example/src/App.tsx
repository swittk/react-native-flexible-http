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
    res.set_content('my content is extremely awesome yahooooooo oh yeah oh yeah baby bring it on! The battle is on bitch');
    console.log('body is', res.body);
  }, []);

  React.useEffect(()=>{
    console.log('about to create server');
    const server = createHTTPServer();
    console.log('createdd server');
    server.get('/', cb);
    server.listen(5002);
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
