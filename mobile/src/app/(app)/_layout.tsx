import { Tabs } from 'expo-router';
import { StyleProp, TextStyle } from 'react-native';

import { IconBook, IconSetting } from '~/assets/icons';
import { AppRoutes } from '~/constants/routes';

const tabBarLabelStyle: StyleProp<TextStyle> = {
  position: 'absolute',
};

// const tabBarStyle = {
//   backgroundColor: appBgColors.primary,
//   height: 40,
// };

const tabBarItemStyle = {
  height: 30,
  marginTop: 5,
};

type BarIconProps = {
  focused: boolean;
  color: string;
  size: number;
};

const hiddenTabOption = {
  href: null,
  headerShown: false,
};

const ExpensesIcon = ({ focused }: BarIconProps) => (
  <IconBook color={focused ? 'primary' : 'secondary'} size={24} />
);

const ProfileTabBarIcon = ({ focused }: BarIconProps) => (
  <IconSetting color={focused ? 'primary' : 'secondary'} size={24} />
);

export default function TabLayout() {
  return (
    <>
      <Tabs
        screenOptions={{
          headerShown: false,
          title: '',
          tabBarActiveTintColor: 'blue',
        }}
      >
        <Tabs.Screen
          name={AppRoutes.OVERVIEW}
          options={{
            tabBarShowLabel: false,
            tabBarLabelStyle,
            tabBarItemStyle,
            tabBarIcon: ExpensesIcon,
          }}
        />
        <Tabs.Screen
          name={AppRoutes.PROFILE}
          options={{
            tabBarShowLabel: false,
            tabBarLabelStyle,
            tabBarItemStyle,
            tabBarIcon: ProfileTabBarIcon,
          }}
        />

        {/* Hidden tab */}
        <Tabs.Screen name={AppRoutes.ACCOUNTS_ADD} options={hiddenTabOption} />
      </Tabs>
    </>
  );
}
