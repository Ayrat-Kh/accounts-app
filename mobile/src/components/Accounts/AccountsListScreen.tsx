import { Text } from '~/components/ui';

import { AccountsListScreenContainer } from './AccountsListScreen.styles';

export const AccountsListScreen = () => {
  return (
    <AccountsListScreenContainer>
      <Text variant="h1" kind="bold" className="mb-28">
        Overview
      </Text>
    </AccountsListScreenContainer>
  );
};
