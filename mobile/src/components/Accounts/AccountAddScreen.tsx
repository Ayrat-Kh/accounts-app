import { useForm } from 'react-hook-form';

import {
  Currency,
  type UpsertAccount as UpsertAccountModel,
} from '~/lib/api/open-api';

import { LayoutProvider } from '../providers';
import { AccountUpsert } from './AccountUpsert';

export const AccountAddScreen = () => {
  const form = useForm<UpsertAccountModel>({
    defaultValues: {
      date: new Date().toISOString(),
      currencyCode: Currency.Usd,
    },
  });
  return (
    <LayoutProvider>
      <AccountUpsert {...form} />
    </LayoutProvider>
  );
};
