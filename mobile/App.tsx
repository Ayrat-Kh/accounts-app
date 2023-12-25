import 'react-native-gesture-handler';

import { StatusBar } from 'expo-status-bar';

import { NavigationContainer } from '~/navigations';

export default function App() {
  return (
    <>
      <NavigationContainer />
      <StatusBar style="auto" />
    </>
  );
}
