import { Redirect } from 'expo-router';
import { ActivityIndicator, View } from 'react-native';

import { useLogoutWatcher } from '~/components/Login/Login.hooks';
import { AppRoutes } from '~/constants/routes';
import { useGetMe } from '~/lib/api/authUser';
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
    return <Redirect href={AppRoutes.OVERVIEW} />;
  }

  return <Redirect href={AppRoutes.LOGIN} />;
};
