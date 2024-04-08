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
  CalendarPicker,
  type CalendarPickerProps,
} from '../ui';

type FormCalendarPickerProps<
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
> = {
  name: TName;
  label: AccessabilityLabel;
  control: Control<TFieldValues>;
} & Omit<CalendarPickerProps, 'onChange' | 'selected' | 'close'>;

export const FormCalendarPicker = <
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
>({
  name,
  label,
  control,
  ...restInputProps
}: FormCalendarPickerProps<TFieldValues, TName>): ReactElement => {
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
        <CalendarPicker
          {...restInputProps}
          label={label}
          error={fieldState.error?.message}
          selected={field.value}
          onChange={(value) => {
            field.onChange(value);
          }}
        />
      )}
    />
  );
};
