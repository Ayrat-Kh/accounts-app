import { Text } from '~/components/ui';

import { ExpensesContainer } from './Expenses.styles';

export const Expenses = () => {
  return (
    <ExpensesContainer>
      <Text variant="h1" kind="bold" className="mb-28">
        Expenses list
      </Text>
    </ExpensesContainer>
  );
};
