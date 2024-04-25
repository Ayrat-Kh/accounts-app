import {
  type Control,
  Controller,
  type ControllerFieldState,
  type ControllerRenderProps,
  type FieldPath,
  type UseFormStateReturn,
} from 'react-hook-form';

import { type AccessabilityLabel } from '~/components/ui';

import { NumberInput, type NumberInputProps } from '../ui/NumberInput';

type FormNumberInputProps<
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
> = {
  name: TName;
  label: AccessabilityLabel;
  control: Control<TFieldValues>;
} & Omit<NumberInputProps, 'onChange'>;

export const FormNumberInput = <
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
>({
  name,
  label,
  control,
  ...restInputProps
}: FormNumberInputProps<TFieldValues, TName>): React.ReactElement => {
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
        <NumberInput
          {...restInputProps}
          label={label}
          value={field.value}
          error={fieldState.error?.message}
          onChange={field.onChange}
        />
      )}
    />
  );
};
