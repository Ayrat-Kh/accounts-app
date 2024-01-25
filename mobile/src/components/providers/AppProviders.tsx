import { QueryClientProvider } from '@tanstack/react-query';
import type { FC } from 'react';
import { SafeAreaProvider } from 'react-native-safe-area-context';

import { queryClient } from '~/lib/api/queryClient';
import { AppNavigationContainer, AppNavigationProvider } from '~/navigations';

import { LayoutProvider } from './LayoutProvider';

export const AppProviders: FC = () => {
  return (
    <QueryClientProvider client={queryClient}>
      <SafeAreaProvider>
        <AppNavigationProvider>
          <LayoutProvider>
            <AppNavigationContainer />
          </LayoutProvider>
        </AppNavigationProvider>
      </SafeAreaProvider>
    </QueryClientProvider>
  );
};
