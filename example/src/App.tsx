import * as React from 'react';

import { StyleSheet, View, Text } from 'react-native';
import { multiply, createHTTPServer } from 'react-native-flexible-http';

export default function App() {
  const [result, setResult] = React.useState<number | undefined>();

  React.useEffect(() => {
    multiply(3, 7).then(setResult);
  }, []);

  React.useEffect(()=>{
    const server = createHTTPServer();
    server.listen(5000);
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
