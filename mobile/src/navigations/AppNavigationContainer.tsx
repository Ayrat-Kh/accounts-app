import { useLoginStore } from '../lib/store';
import { BottomTabNavigator } from './BottomTabNavigator';
import { LoginNavigator } from './LoginNavigator';

export const AppNavigationContainer = () => {
  const isSignedIn = useLoginStore((state) => state.isSignedIn);

  return <>{isSignedIn ? <BottomTabNavigator /> : <LoginNavigator />}</>;
};
