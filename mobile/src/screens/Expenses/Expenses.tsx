import { IconGoogle } from '~/assets/icons/icon-google';
import { Button, Text } from '~/components/ui';
import { useGoogleSsoAuth } from '~/hooks/useGoogleSsoAuth';

import { ExpensesContainer } from './Expenses.styles';

export const Expenses = () => {
  const googleLogin = useGoogleSsoAuth();

  return (
    <ExpensesContainer>
      <Text variant="h1" kind="bold" className="mb-28">
        Expenso
      </Text>
      <Button variant="ghost" onPress={googleLogin}>
        <IconGoogle color="primary" />
      </Button>
    </ExpensesContainer>
  );
};
