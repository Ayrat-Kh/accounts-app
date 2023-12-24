import 'react-native-gesture-handler';
import React from 'react';
import { StatusBar } from 'expo-status-bar';
import { NavigationContainer } from './src/navigation';

export default function App() {
    return (
        <>
            <NavigationContainer />
            <StatusBar style="auto" />
        </>
    );
}
