import { QueryClientProvider } from '@tanstack/react-query';
import type { FC, PropsWithChildren } from 'react';
import { StatusBar } from 'react-native';
import { SafeAreaProvider } from 'react-native-safe-area-context';

import { queryClient } from '~/lib/api/queryClient';

type AppProviderInnerProps = PropsWithChildren;

export const AppProviderInner: FC<AppProviderInnerProps> = ({ children }) => {
  return (
    <QueryClientProvider client={queryClient}>
      <SafeAreaProvider>
        <>
          {children}
          <StatusBar />
        </>
      </SafeAreaProvider>
    </QueryClientProvider>
  );
};
