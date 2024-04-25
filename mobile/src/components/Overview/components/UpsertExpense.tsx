import { FC } from 'react';
import { FormProvider, type UseFormReturn } from 'react-hook-form';
import { ScrollView, View } from 'react-native';

import {
  FormCalendarPicker,
  FormInput,
  FormSelectPicker,
} from '~/components/form';
import type { UpsertAccount } from '~/lib/api/open-api';

import { MapInput } from './MapInput';

type UpsertExpenseProps = UseFormReturn<UpsertAccount>;

export const UpsertExpense: FC<UpsertExpenseProps> = ({ ...form }) => {
  const { control } = form;

  return (
    <FormProvider {...form}>
      <ScrollView className="py-4">
        <FormInput name="name" label="Name" control={control} />

        <View className="flex-row w-full mt-2 gap-2">
          <FormInput
            name="value"
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

        <MapInput />
      </ScrollView>
    </FormProvider>
  );
};
