import type { ReactElement } from 'react';
import {
  Control,
  Controller,
  type ControllerFieldState,
  type ControllerRenderProps,
  type FieldPath,
  type UseFormStateReturn,
} from 'react-hook-form';

import {
  type AccessebilityLabel,
  SelectPicker,
  type SelectPickerProps,
} from '../ui';

type FormSelectProps<
  TValue extends string | number,
  TMultiple extends boolean,
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
> = {
  name: TName;
  label: AccessebilityLabel;
  control: Control<TFieldValues>;
} & Omit<
  SelectPickerProps<unknown, TValue, TMultiple>,
  'onChange' | 'selected'
>;

export const FormSelect = <
  TValue extends string | number,
  TMultiple extends boolean,
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
>({
  name,
  label,
  control,
  ...restInputProps
}: FormSelectProps<TValue, TMultiple, TFieldValues, TName>): ReactElement => {
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
          isMultiple={false}
          label={label}
          error={fieldState.error?.message}
          selected={field.value}
          onChange={() => {
            // field.onChange(text);
          }}
        />
      )}
    />
  );
};
