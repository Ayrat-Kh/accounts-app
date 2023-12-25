import React from 'react';
import {
  createStackNavigator,
  type StackNavigationOptions,
} from '@react-navigation/stack';
import { View } from 'react-native';

const Stack = createStackNavigator();

const loginOptions: StackNavigationOptions = {
  header: () => null,
};

export const LoginNavigator = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen name="Login" component={View} options={loginOptions} />
    </Stack.Navigator>
  );
};
