import { ScreenSafeLayout } from '~/components/ScreenSafeLayout';
import { Text } from '~/components/ui/Text';

import { AccountAddButton } from '../Accounts';

export const OverviewScreen = () => {
  return (
    <ScreenSafeLayout>
      <Text kind="bold">Overview</Text>
      <AccountAddButton />
    </ScreenSafeLayout>
  );
};
