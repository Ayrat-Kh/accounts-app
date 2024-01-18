import { Text } from '~/components/ui';

import { ExpensesUpsertContainer } from './ExpensesUpsert.styles';

export const ExpensesUpsert = () => {
  return (
    <ExpensesUpsertContainer>
      <Text variant="h1" kind="bold" className="mb-28">
        Create expenses
      </Text>
    </ExpensesUpsertContainer>
  );
};
