import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';

import { Expenses } from '~/screens/Expenses';
import { Profile } from '~/screens/Profile';

const Tab = createBottomTabNavigator();

export const BottomTabNavigator = () => {
  return (
    <Tab.Navigator>
      <Tab.Screen name="Expenses" component={Expenses} />
      <Tab.Screen name="Profile" component={Profile} />
    </Tab.Navigator>
  );
};
