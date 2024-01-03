import { Button, Text } from '~/components/ui';
import { useGoogleSsoAuth } from '~/hooks/useGoogleSsoAuth';

import { LoginContainer } from './Login.styles';

export const Login = () => {
  const googleLogin = useGoogleSsoAuth();

  const handleLogin = async () => {
    const user = await googleLogin();
    console.log('user', user);
  };

  return (
    <LoginContainer>
      <Button onPress={handleLogin}>
        <Text>Google login 1</Text>
      </Button>
    </LoginContainer>
  );
};
