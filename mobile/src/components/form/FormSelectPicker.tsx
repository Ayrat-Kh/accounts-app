import type { ReactElement } from 'react';
import {
  type Control,
  Controller,
  type ControllerFieldState,
  type ControllerRenderProps,
  type FieldPath,
  type UseFormStateReturn,
} from 'react-hook-form';

import {
  type AccessabilityLabel,
  SelectPicker,
  type SelectPickerProps,
} from '../ui';

type FormSelectPickerProps<
  TValue extends string | number,
  TMultiple extends boolean,
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
> = {
  name: TName;
  label: AccessabilityLabel;
  control: Control<TFieldValues>;
} & Omit<
  SelectPickerProps<unknown, TValue, TMultiple>,
  'onChange' | 'selected'
>;

export const FormSelectPicker = <
  TValue extends string | number,
  TMultiple extends boolean,
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
>({
  name,
  label,
  control,
  isMultiple,
  ...restInputProps
}: FormSelectPickerProps<
  TValue,
  TMultiple,
  TFieldValues,
  TName
>): ReactElement => {
  return (
    <Controller
      control={control}
      name={name}
      render={({
        field,
        fieldState,
      }: {
        field: ControllerRenderProps<TFieldValues, TName>;
        fieldState: ControllerFieldState;
        formState: UseFormStateReturn<TFieldValues>;
      }) => (
        <SelectPicker
          {...restInputProps}
          isMultiple={isMultiple}
          label={label}
          error={fieldState.error?.message}
          selected={field.value}
          onChange={(value: unknown) => {
            field.onChange(value);
          }}
        />
      )}
    />
  );
};
