import { Text } from '~/components/ui';

import { ExpensesUpsert } from './ExpensesUpsert.styles';

export const Expenses = () => {
  return (
    <ExpensesUpsert>
      <Text variant="h1" kind="bold" className="mb-28">
        Create expenses
      </Text>
    </ExpensesUpsert>
  );
};
