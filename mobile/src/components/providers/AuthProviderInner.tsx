import { Redirect } from 'expo-router';
import { ActivityIndicator, View } from 'react-native';

import { useLogoutWatcher } from '~/components/Login/Login.hooks';
import { useGetMe } from '~/lib/api/auth-user';
import { useLoginStore } from '~/lib/store';

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
    return <Redirect href="overview" />;
  }

  return <Redirect href="login" />;
};
