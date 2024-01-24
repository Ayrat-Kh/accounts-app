import React from 'react';
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
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
  TLabel extends AccessebilityLabel = string,
> = {
  name: TName;
  label: TLabel;
  control: Control<TFieldValues>;
} & SelectPickerProps<TLabel>;

export const FormSelect = <
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
  TLabel extends AccessebilityLabel = string,
>({
  name,
  label,
  control,
  ...restInputProps
}: FormSelectProps<TFieldValues, TName, TLabel>): React.ReactElement => {
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
        <SelectPicker<TFieldValues[TName]>
          {...restInputProps}
          label={label}
          error={fieldState.error?.message}
          onChange={(text) => {
            field.onChange(text);
          }}
        />
      )}
    />
  );
};
