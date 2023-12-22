import { NavigationContainer as NativeNavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import { useLoginStore } from '../lib/store';

const Stack = createStackNavigator();

export const NavigationContainer = () => {
    // const {} = useLoginStore(state => state.);

    return (
        <NativeNavigationContainer>
            <Stack.Navigator>
                <Stack.Screen name="Home" component={Home} />
            </Stack.Navigator>
        </NativeNavigationContainer>
    );
};
