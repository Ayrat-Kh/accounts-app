import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import { type StyleProp, type TextStyle, View } from 'react-native';

import { IconBook, IconSetting } from '~/assets/icons';
import { appColors } from '~/components/ui/colors';
import { AddExpenseModalButton, Expenses } from '~/screens/Expenses';
import { Profile } from '~/screens/Profile';

const tabBarLabelStyle: StyleProp<TextStyle> = {
  position: 'absolute',
};
const color = appColors['app-primary'][200];

const tabBarStyle = {
  backgroundColor: color,
  height: 40,
};

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

const Tab = createBottomTabNavigator();

export const BottomTabNavigator = () => {
  return (
    <Tab.Navigator
      screenOptions={{
        tabBarStyle,
      }}
    >
      <Tab.Screen
        name="Expenses"
        component={Expenses}
        options={{
          headerShown: false,
          tabBarShowLabel: false,
          tabBarLabelStyle,
          tabBarItemStyle,
          tabBarIcon: ExpensesIcon,
        }}
      />
      <Tab.Screen
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
      />
      <Tab.Screen
        name="Profile"
        component={Profile}
        options={{
          title: 'Profile',
          tabBarShowLabel: false,
          tabBarLabelStyle,
          tabBarItemStyle,
          tabBarIcon: ProfileTabBarIcon,
        }}
      />
    </Tab.Navigator>
  );
};
