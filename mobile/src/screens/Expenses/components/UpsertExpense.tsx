import { useForm } from 'react-hook-form';
import { View } from 'react-native';

import { FormInput } from '~/components/form/FormInput';
import { FormSelect } from '~/components/form/FormSelectPicker';
import type { ExpensesExpenseDto } from '~/lib/api/open-api';

export const UpsertExpense = () => {
  const { control } = useForm<ExpensesExpenseDto>();

  return (
    <View>
      <FormInput name="name" label="Name" control={control} />
      {/* <View className=" flex-row bg-red-500"> */}
      <FormInput
        name="total"
        label="Price"
        isNumeric
        isDecimal
        control={control}
      />
      <FormSelect
        control={control}
        name="currencyCode"
        label="Currency"
        isMultiple={false}
        options={[
          {
            label: 'USD',
            value: 'USD',
            data: 'USD',
          },
          {
            label: 'EUR',
            value: 'EUR',
            data: 'USD',
          },
        ]}
      />
      {/* </View> */}
    </View>
  );
};
