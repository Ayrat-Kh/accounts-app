import { useForm } from 'react-hook-form';
import { View } from 'react-native';

import {
  FormCalendarPicker,
  FormInput,
  FormSelectPicker,
} from '~/components/form';
import { Map } from '~/components/ui';
import type { ExpensesExpenseDto } from '~/lib/api/open-api';

export const UpsertExpense = () => {
  const { control } = useForm<ExpensesExpenseDto>();

  return (
    <View>
      <FormInput name="name" label="Name" control={control} />

      <View className="flex-row w-full mt-2 gap-2">
        <FormInput
          name="total"
          label="Price"
          isNumeric
          isDecimal
          control={control}
          className="grow-[4]"
        />
        <FormSelectPicker
          className="grow-[1]"
          control={control}
          name="currencyCode"
          label="Currency"
          placeholder="Select currency"
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
      </View>
      <FormCalendarPicker
        control={control}
        name="date"
        label="Date"
        className="mt-2"
        placeholder="Select date"
      />
      <Map />
    </View>
  );
};
