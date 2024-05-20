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
  Input,
  type InputProps,
} from '~/components/ui';

type FormInputProps<
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
> = {
  name: TName;
  label: AccessabilityLabel;
  control: Control<TFieldValues>;
} & Omit<InputProps, 'onChange'>;

export const FormInput = <
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  TFieldValues extends Record<string, any> = Record<string, any>,
  TName extends FieldPath<TFieldValues> = FieldPath<TFieldValues>,
>({
  name,
  label,
  control,
  ...restInputProps
}: FormInputProps<TFieldValues, TName>): React.ReactElement => {
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
          value={field.value}
          error={fieldState.error?.message}
          onChange={field.onChange}
        />
      )}
    />
  );
};
