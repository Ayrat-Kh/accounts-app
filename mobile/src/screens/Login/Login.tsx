import { Button, Text } from '~/components/ui';
import { useGoogleSsoAuth } from '~/hooks/useGoogleSsoAuth';

import { LoginContainer } from './Login.styles';

export const Login = () => {
  const googleLogin = useGoogleSsoAuth();

  return (
    <LoginContainer>
      <Button onPress={googleLogin}>
        <Text>Google login</Text>
      </Button>
    </LoginContainer>
  );
};
