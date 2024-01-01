import { Button, Text } from '~/components/ui';

import { LoginContainer } from './Login.styles';

export const Login = () => {
  return (
    <LoginContainer>
      <Button>
        <Text>Google login</Text>
      </Button>
    </LoginContainer>
  );
};
