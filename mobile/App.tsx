import 'react-native-gesture-handler';

import { StatusBar } from 'expo-status-bar';

import { AppProviders } from '~/components/providers';

export default function App() {
  return (
    <>
      <AppProviders />
      <StatusBar style="auto" />
    </>
  );
}
