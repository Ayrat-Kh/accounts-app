import { router } from 'expo-router';
import { View } from 'react-native';

import { AppRoutes } from '~/constants/routes';
import { useLoginStore } from '~/lib/store';

import { ScreenSafeLayout } from '../ScreenSafeLayout';
import { Button } from '../ui';

export const ProfileScreen = () => {
  const handleLogout = () => {
    useLoginStore.getState().logout();
    router.replace(AppRoutes.LOGIN.path);
  };

  return (
    <ScreenSafeLayout>
      <View>
        <Button variant="secondary" onPress={handleLogout}>
          Logout
        </Button>
      </View>
    </ScreenSafeLayout>
  );
};
