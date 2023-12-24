import React from 'react';
import { NavigationContainer as NativeNavigationContainer } from '@react-navigation/native';

import { useLoginStore } from '../lib/store';
import { BottomTabNavigator } from './BottomTabNavigator';
import { LoginNavigator } from './LoginNavigator';

export const NavigationContainer = () => {
    const isSignedIn = useLoginStore((state) => state.isSignedIn);

    return (
        <NativeNavigationContainer>
            {isSignedIn ? <BottomTabNavigator /> : <LoginNavigator />}
        </NativeNavigationContainer>
    );
};
