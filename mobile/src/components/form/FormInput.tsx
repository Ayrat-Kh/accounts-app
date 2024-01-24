import React from 'react';
import {
  Control,
  Controller,
  type ControllerFieldState,
  type ControllerRenderProps,
  type FieldPath,
  type UseFormStateReturn,
} from 'react-hook-form';

import { type AccessebilityLabel, Input, InputProps } from '../ui';

type FormInputProps<
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
  TLabel extends AccessebilityLabel = string,
> = {
  name: TName;
  label: TLabel;
  control: Control<TFieldValues>;
} & InputProps<TLabel>;

export const FormInput = <
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
  TLabel extends AccessebilityLabel = string,
>({
  name,
  label,
  control,
  ...restInputProps
}: FormInputProps<TFieldValues, TName, TLabel>): React.ReactElement => {
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
        <Input
          {...restInputProps}
          label={label}
          error={fieldState.error?.message}
          onChangeText={(text) => {
            if (
              ['number-pad', 'decimal-pad', 'numeric'].includes(
                restInputProps.keyboardType!,
              )
            ) {
              field.onChange(parseFloat(text));
            } else {
              field.onChange(text);
            }
          }}
        />
      )}
    />
  );
};
