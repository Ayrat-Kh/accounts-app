import { IconGoogle } from '~/assets/icons/icon-google';
import { Button, Text } from '~/components/ui';
import { useGoogleSsoAuth } from '~/hooks/useGoogleSsoAuth';

import { LoginContainer } from './LoginInner.styles';

export const LoginInner = () => {
  const googleLogin = useGoogleSsoAuth();

  return (
    <LoginContainer>
      <Text variant="h1" kind="bold" className="mb-28">
        Accounts
      </Text>
      <Button variant="ghost" onPress={googleLogin}>
        <IconGoogle color="primary" size={40} />
      </Button>
    </LoginContainer>
  );
};
