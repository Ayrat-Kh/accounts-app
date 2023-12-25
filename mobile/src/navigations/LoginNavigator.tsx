import {
  type StackNavigationOptions,
  createStackNavigator,
} from '@react-navigation/stack';

import { Login } from '~/screens/Login';

const Stack = createStackNavigator();

const loginOptions: StackNavigationOptions = {
  header: () => null,
};

export const LoginNavigator = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen name="Login" component={Login} options={loginOptions} />
    </Stack.Navigator>
  );
};
