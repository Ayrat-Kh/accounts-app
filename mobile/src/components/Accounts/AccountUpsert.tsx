import { FC } from 'react';
import { FormProvider, type UseFormReturn } from 'react-hook-form';
import { ScrollView, View } from 'react-native';

import {
  FormCalendarPicker,
  FormInput,
  FormSelectPicker,
} from '~/components/form';
import {
  Currency,
  type UpsertAccount as UpsertAccountModel,
} from '~/lib/api/open-api';

import { AccountUpsertMapInput } from './AccountUpsertMapInput';

type AccountUpsertProps = UseFormReturn<UpsertAccountModel>;

export const AccountUpsert: FC<AccountUpsertProps> = ({ ...form }) => {
  const { control } = form;

  return (
    <FormProvider {...form}>
      <ScrollView className="p-2">
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
            options={Object.values(Currency).map((currency) => ({
              label: currency,
              value: currency,
              data: currency,
            }))}
          />
        </View>
        <FormCalendarPicker
          control={control}
          name="date"
          label="Date"
          className="mt-2"
          placeholder="Select date"
        />

        <AccountUpsertMapInput />
      </ScrollView>
    </FormProvider>
  );
};
