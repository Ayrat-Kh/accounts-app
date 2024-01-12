import { ActivityIndicator } from 'react-native';

import { useGetMe } from '~/lib/api/auth-user';

import { useLoginStore, useUserStore } from '../lib/store';
import { useLogoutWatcher } from './AppNavigationContainer.hooks';
import { BottomTabNavigator } from './BottomTabNavigator';
import { LoginNavigator } from './LoginNavigator';

const AppNavigationContainerInner = () => {
  const isSignedIn = useLoginStore((state) => state.isSignedIn);

  if (isSignedIn) {
    return <BottomTabNavigator />;
  }

  return <LoginNavigator />;
};

export const AppNavigationContainer = () => {
  const accessToken = useLoginStore((state) => state.accessToken);
  const isLoginStoreLoaded = useLoginStore((state) => state._hasHydrated);
  const isUserStoreLoaded = useUserStore((state) => state._hasHydrated);
  const { isLoading: isLoadingMeInfo } = useGetMe({
    enabled: Boolean(accessToken),
  });

  useLogoutWatcher();

  if (
    !isLoginStoreLoaded ||
    !isUserStoreLoaded ||
    (Boolean(accessToken) && isLoadingMeInfo)
  ) {
    return <ActivityIndicator />;
  }

  return <AppNavigationContainerInner />;
};
