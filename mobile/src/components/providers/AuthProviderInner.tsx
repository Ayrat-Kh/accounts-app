import { Redirect } from 'expo-router';
import { ActivityIndicator, View } from 'react-native';

import { useGetMe } from '~/lib/api/auth-user';
import { useLoginStore } from '~/lib/store';

import { useLogoutWatcher } from '../Login/Login.hooks';

export const AuthProviderInner = () => {
  const accessToken = useLoginStore((state) => state.accessToken);
  const isLoginStoreLoaded = useLoginStore((state) => state._hasHydrated);
  const isSignedIn = useLoginStore((state) => state.isSignedIn);

  const { isLoading: isLoadingMeInfo } = useGetMe({
    enabled: Boolean(accessToken),
  });

  useLogoutWatcher();

  if (!isLoginStoreLoaded || (Boolean(accessToken) && isLoadingMeInfo)) {
    return (
      <View className="flex-1 justify-center">
        <ActivityIndicator size="large" />
      </View>
    );
  }

  if (isSignedIn) {
    return <Redirect href="(tabs)" />;
  }

  return <Redirect href="login" />;
};
