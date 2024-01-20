import {
  type BottomTabBarButtonProps,
  createBottomTabNavigator,
} from '@react-navigation/bottom-tabs';
import { type StyleProp, type TextStyle, View } from 'react-native';

import { IconAddOutlinedNoBorder } from '~/assets/icons/icon-add-outlined-no-border';
import { IconBook } from '~/assets/icons/icon-book';
import { IconSetting } from '~/assets/icons/icon-setting';
import { Button } from '~/components/ui';
import { appColors } from '~/components/ui/colors';
import { Expenses, ExpensesUpsert } from '~/screens/Expenses';
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

const ExpenseCreateTabBarButton: React.FC<BottomTabBarButtonProps> = () => {
  return (
    <View className="top-[-28] h-[56] max-w-[56] flex-1 justify-center items-center rounded-full bg-app-primary-200">
      <Button variant="primary" rounded maxWidth>
        <IconAddOutlinedNoBorder color="primary" size={40} />
      </Button>
    </View>
  );
};
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
        component={ExpensesUpsert}
        options={{
          tabBarActiveTintColor: 'red',
          headerShown: false,
          tabBarShowLabel: false,
          tabBarLabelStyle,
          tabBarItemStyle,
          tabBarButton: ExpenseCreateTabBarButton,
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
