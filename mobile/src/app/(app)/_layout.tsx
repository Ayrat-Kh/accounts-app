import { Tabs } from 'expo-router';

import { IconBook, IconSetting } from '~/assets/icons';
import { AppRoutes } from '~/constants/routes';
import { useBgColor } from '~/ui';

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
};

const ExpensesIcon = ({ focused }: BarIconProps) => (
  <IconBook color={focused ? 'primary' : 'secondary'} size={24} />
);

const ProfileTabBarIcon = ({ focused }: BarIconProps) => (
  <IconSetting color={focused ? 'primary' : 'secondary'} size={24} />
);

export default function TabLayout() {
  const tabBarBg = useBgColor('compSecondary');
  const sectionBg = useBgColor('secondary');

  return (
    <>
      <Tabs
        sceneContainerStyle={{
          backgroundColor: sectionBg,
        }}
        screenOptions={{
          headerShown: false,
          title: '',
          tabBarItemStyle,
          tabBarShowLabel: false,
          tabBarStyle: {
            backgroundColor: tabBarBg,
          },
        }}
      >
        <Tabs.Screen
          name={AppRoutes.OVERVIEW}
          options={{
            tabBarIcon: ExpensesIcon,
          }}
        />
        <Tabs.Screen
          name={AppRoutes.PROFILE}
          options={{
            tabBarIcon: ProfileTabBarIcon,
          }}
        />

        {/* Hidden tab */}
        <Tabs.Screen
          name={AppRoutes.ACCOUNTS_ADD}
          options={{
            ...hiddenTabOption,
          }}
        />
      </Tabs>
    </>
  );
}
