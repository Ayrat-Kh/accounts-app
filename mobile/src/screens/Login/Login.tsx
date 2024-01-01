import {
  GoogleSignin,
  GoogleSigninButton,
} from '@react-native-google-signin/google-signin';

import { Text } from '~/components/ui';

import { LoginContainer } from './Login.styles';

GoogleSignin.configure();

export const Login = () => {
  const handleLogin = async () => {
    try {
      const userInfo = await GoogleSignin.signIn();
      console.log('userInfo', userInfo);
    } catch (error) {
      // no code
    }
  };

  return (
    <LoginContainer>
      <GoogleSigninButton
        size={GoogleSigninButton.Size.Wide}
        color={GoogleSigninButton.Color.Light}
        onPress={handleLogin}
      ></GoogleSigninButton>
    </LoginContainer>
  );
};
