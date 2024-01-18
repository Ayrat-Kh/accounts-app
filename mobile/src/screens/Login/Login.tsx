import { IconGoogle } from '~/assets/icons/icon-google';
import { Button, Text } from '~/components/ui';
import { useGoogleSsoAuth } from '~/hooks/useGoogleSsoAuth';

import { LoginContainer } from './Login.styles';

export const Login = () => {
  const googleLogin = useGoogleSsoAuth();

  return (
    <LoginContainer>
      <Text variant="h1" kind="bold" className="mb-28">
        Expenso
      </Text>
      <Button variant="ghost" onPress={googleLogin}>
        <IconGoogle color="primary" />
      </Button>
    </LoginContainer>
  );
};
