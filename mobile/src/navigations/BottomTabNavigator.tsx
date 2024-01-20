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

const ExpenseCreateTabBarIcon = ({ focused }: BarIconProps) => (
  <IconAddOutlinedNoBorder
    color={focused ? 'primary' : 'secondary'}
    size={40}
  />
);

const ProfileTabBarIcon = ({ focused }: BarIconProps) => (
  <IconSetting color={focused ? 'primary' : 'secondary'} size={24} />
);

const ExpenseCreateTabBarButton = (props: BottomTabBarButtonProps) => {
  return (
    <View className="mb-1 top-[-30] h-16 w-16 rounded-full bg-app-primary-200 border-app-secondary-300">
      <Button variant="ghost" className=" " {...props}>
        {props.children}
      </Button>
    </View>
  );
};
const Tab = createBottomTabNavigator();

export const BottomTabNavigator = () => {
  return (
    <Tab.Navigator
      sceneContainerStyle={{}}
      screenOptions={{
        tabBarStyle: {
          backgroundColor: color,
          height: 40,
        },
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
          tabBarIcon: ExpenseCreateTabBarIcon,
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
