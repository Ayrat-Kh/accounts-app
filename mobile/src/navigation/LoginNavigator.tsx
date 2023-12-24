import React from 'react';
import { createStackNavigator } from '@react-navigation/stack';
import { View } from 'react-native';

const Stack = createStackNavigator();

export const LoginNavigator = () => {
    return (
        <Stack.Navigator>
            <Stack.Screen name="Home" component={View} />
        </Stack.Navigator>
    );
};
