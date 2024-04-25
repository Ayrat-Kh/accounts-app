import { useForm } from 'react-hook-form';

import { type Account, Currency } from '~/lib/api/open-api';

import { AccountUpsert } from './AccountUpsert';

export const AccountAddScreen = () => {
  const form = useForm<Account>({
    defaultValues: {
      date: new Date().toISOString(),
      currencyCode: Currency.Usd,
    },
  });
  return <AccountUpsert {...form} />;
};
