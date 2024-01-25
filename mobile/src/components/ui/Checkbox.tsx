import CheckboxRN, {
  type CheckboxProps as CheckboxRNProps,
} from 'expo-checkbox';
import type { FC, ReactNode } from 'react';
import { View } from 'react-native';

import { Text } from './Text';

type CheckboxProps = {
  label: string | ReactNode;
  isChecked: boolean;
  className?: string;
  onChange: (isChecked: boolean) => void;
} & Omit<CheckboxRNProps, 'onChange'>;

export const Checkbox: FC<CheckboxProps> = ({
  label,
  isChecked,
  className,
  onChange,
}: CheckboxProps) => {
  const classNames = ['flex-row', className];

  return (
    <View className={classNames.join(' ')}>
      <CheckboxRN value={isChecked} onValueChange={onChange} />
      {typeof label === 'string' ? (
        <Text className="ml-1">{label}</Text>
      ) : (
        label
      )}
    </View>
  );
};
