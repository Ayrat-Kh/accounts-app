import {
  BottomTabBarButtonProps,
  createBottomTabNavigator,
} from '@react-navigation/bottom-tabs';
import { View } from 'react-native';

import { IconAdd } from '~/assets/icons/icon-add';
import { Button } from '~/components/ui';
import { Expenses } from '~/screens/Expenses';
import { ExpensesUpsert } from '~/screens/Expenses/ExpensesUpsert.styles';
import { Profile } from '~/screens/Profile';

const Tab = createBottomTabNavigator();

export const BottomTabNavigator = () => {
  return (
    <Tab.Navigator>
      <Tab.Screen name="Expenses" component={Expenses} />
      <Tab.Screen
        name="ExpensesUpsert"
        component={ExpensesUpsert}
        options={{
          headerShown: false,
          title: '+',
          tabBarButton: (props: BottomTabBarButtonProps) => (
            <Button variant="primary" {...props}>
              {props.children}
            </Button>
          ),
          tabBarIcon: () => (
            <View className="mb-3 h-24 bg-red-400">
              <IconAdd width={75} height={75} color="primary" />
            </View>
          ),
        }}
      />
      <Tab.Screen name="Profile" component={Profile} />
    </Tab.Navigator>
  );
};
