import { Text } from '~/components/ui/Text';

import { AccountAddButton } from '../Accounts';
import { LayoutProvider } from '../providers';

export const OverviewScreen = () => {
  return (
    <LayoutProvider>
      <Text kind="bold">Overview</Text>
      <AccountAddButton />
    </LayoutProvider>
  );
};
