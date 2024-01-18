import {
  type BottomTabBarButtonProps,
  createBottomTabNavigator,
} from '@react-navigation/bottom-tabs';
import { type StyleProp, type TextStyle, View } from 'react-native';

import { IconAddOutlinedNoBorder } from '~/assets/icons/icon-add-outlined-no-border';
import { IconBook } from '~/assets/icons/icon-book';
import { IconSetting } from '~/assets/icons/icon-setting';
import { Button } from '~/components/ui';
import { Expenses, ExpensesUpsert } from '~/screens/Expenses';
import { Profile } from '~/screens/Profile';

const Tab = createBottomTabNavigator();
const tabBarLabelStyle: StyleProp<TextStyle> = {
  position: 'absolute',
};
const color = '#F5FBF5'; // fix me

const tabBarItemStyle = {
  height: 30,
  marginTop: 5,
};

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
          tabBarIcon: () => <IconBook color="secondary" size={24} />,
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
          tabBarButton: (props: BottomTabBarButtonProps) => (
            <View className="mb-1 top-[-30] h-16 w-16 rounded-full bg-app-primary-200 border-app-secondary-300">
              <Button variant="ghost" className=" " {...props}>
                {props.children}
              </Button>
            </View>
          ),
          tabBarIcon: () => (
            <IconAddOutlinedNoBorder color="secondary" size={40} />
          ),
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
          tabBarIcon: () => <IconSetting color="secondary" size={24} />,
        }}
      />
    </Tab.Navigator>
  );
};
