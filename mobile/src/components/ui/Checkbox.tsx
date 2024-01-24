import CheckboxRN, {
  type CheckboxProps as CheckboxRNProps,
} from 'expo-checkbox';
import type { FC, ReactNode } from 'react';

import { Text } from './Text';

type CheckboxProps = {
  label: string | ReactNode;
  isChecked: boolean;
  onChange: (isChecked: boolean) => void;
} & Omit<CheckboxRNProps, 'onChange'>;

export const Checkbox: FC<CheckboxProps> = ({
  label,
  isChecked,
  onChange,
}: CheckboxProps) => {
  return (
    <CheckboxRN value={isChecked} onValueChange={onChange}>
      {typeof label === 'string' ? <Text>{label}</Text> : label}
    </CheckboxRN>
  );
};
