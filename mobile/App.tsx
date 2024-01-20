import 'react-native-gesture-handler';

import { StatusBar } from 'expo-status-bar';
import 'react-native-url-polyfill/auto';

import { AppProviders } from '~/components/providers';
import { axiosInstance } from '~/lib/api/axios';
import { registerAccessTokenHeader } from '~/lib/api/interceptors/register-acess-token-header';

import './src/global.css';

registerAccessTokenHeader(axiosInstance);

export default function App() {
  return (
    <>
      <AppProviders />
      <StatusBar style="auto" />
    </>
  );
}
