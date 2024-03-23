import { useForm } from 'react-hook-form';

import { ExpensesExpenseDto } from '~/lib/api/open-api';

import { UpsertExpense } from './UpsertExpense';

export const CreateExpense = () => {
  const form = useForm<ExpensesExpenseDto>({
    defaultValues: {
      date: new Date(),
      currencyCode: 'USD',
    },
  });
  return <UpsertExpense {...form} />;
};
