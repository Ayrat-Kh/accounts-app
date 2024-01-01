import { SafeAreaProvider } from 'react-native-safe-area-context';

import { AppNavigationContainer, AppNavigationProvider } from '~/navigations';

import { LayoutProvider } from './LayoutProvider';

export const AppProviders: React.FC = () => {
  return (
    <SafeAreaProvider>
      <AppNavigationProvider>
        <LayoutProvider>
          <AppNavigationContainer />
        </LayoutProvider>
      </AppNavigationProvider>
    </SafeAreaProvider>
  );
};
