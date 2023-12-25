import { createStackNavigator } from '@react-navigation/stack';
import { View } from 'react-native';

const Stack = createStackNavigator();

export const BottomTabNavigator = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen name="Home" component={View} />
    </Stack.Navigator>
  );
};
