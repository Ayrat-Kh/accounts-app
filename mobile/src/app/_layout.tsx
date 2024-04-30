import {
  Inter_400Regular,
  Inter_500Medium,
  Inter_700Bold,
  useFonts,
} from '@expo-google-fonts/inter';
import { SplashScreen, Stack } from 'expo-router';
import { useEffect } from 'react';

import { AppProvider } from '~/components/providers';
import { AppRoutes } from '~/constants/routes';
import '~/global.css';
import { axiosInstance } from '~/lib/api/axios';
import { registerAccessTokenHeader } from '~/lib/api/interceptors/register-access-token-header';

registerAccessTokenHeader(axiosInstance);

export default function AppLayout() {
  const [fontsLoaded, fontError] = useFonts({
    Inter_400Regular,
    Inter_500Medium,
    Inter_700Bold,
  });

  useEffect(() => {
    if (fontsLoaded || fontError) {
      // Hide the splash screen after the fonts have loaded (or an error was returned) and the UI is ready.
      SplashScreen.hideAsync();
    }
  }, [fontsLoaded, fontError]);

  // Prevent rendering until the font has loaded or an error was returned
  if (!fontsLoaded && !fontError) {
    return null;
  }

  // Render the children routes now that all the assets are loaded.
  return (
    <AppProvider>
      <Stack screenOptions={{ headerShown: false, headerBackVisible: false }}>
        <Stack.Screen
          name={AppRoutes.LOGIN.name}
          options={{ headerShown: true, headerTitle: 'Login' }}
        />
        <Stack.Screen name={AppRoutes.APP_TAB.name} />
      </Stack>
    </AppProvider>
  );
}
