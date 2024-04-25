import { Tabs } from 'expo-router';
// import React from 'react';
import { StyleProp, TextStyle } from 'react-native';

import { IconBook, IconSetting } from '~/assets/icons';

// import { appBgColors } from '~/ui';

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

const ExpensesIcon = ({ focused }: BarIconProps) => (
  <IconBook color={focused ? 'primary' : 'secondary'} size={24} />
);

const ProfileTabBarIcon = ({ focused }: BarIconProps) => (
  <IconSetting color={focused ? 'primary' : 'secondary'} size={24} />
);

export default function TabLayout() {
  return (
    <Tabs screenOptions={{ tabBarActiveTintColor: 'blue' }}>
      <Tabs.Screen
        name="overview"
        options={{
          headerShown: false,
          tabBarShowLabel: false,
          tabBarLabelStyle,
          tabBarItemStyle,
          tabBarIcon: ExpensesIcon,
        }}
      />
      {/* <Tabs.Screen
        name="ExpensesUpsert"
        component={View} // stub
        options={{
          tabBarActiveTintColor: 'red',
          headerShown: false,
          tabBarShowLabel: false,
          tabBarLabelStyle,
          tabBarItemStyle,
          tabBarButton: AddExpenseModalButton,
          tabBarIcon: () => null,
        }}
      /> */}
      <Tabs.Screen
        name="profile"
        options={{
          title: 'Profile',
          tabBarShowLabel: false,
          tabBarLabelStyle,
          tabBarItemStyle,
          tabBarIcon: ProfileTabBarIcon,
        }}
      />
    </Tabs>
  );
}
