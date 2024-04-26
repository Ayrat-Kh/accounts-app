import { useNavigation } from 'expo-router';
import { View } from 'react-native';

import { IconAddOutlinedNoBorder } from '~/assets/icons';
import { Button } from '~/components/ui';
import { AppNavigationProp, AppRoutes } from '~/constants/routes';

export const AccountAddButton = () => {
  const { navigate } = useNavigation<AppNavigationProp>();

  const handlePress = () => {
    navigate(AppRoutes.ACCOUNTS_ADD);
  };

  return (
    <View className="absolute bottom-[25] right-[15] justify-center items-center rounded-full bg-app-primary-200">
      <Button
        variant="secondary"
        rounded
        size="any"
        icon={<IconAddOutlinedNoBorder size={25} />}
        onPress={handlePress}
      />
    </View>
  );
};
