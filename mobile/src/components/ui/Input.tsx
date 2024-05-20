import type { ReactNode } from 'react';
import {
  type TextInputProps as TextInputPropsRN,
  TextInput as TextInputRN,
  View,
} from 'react-native';

import { InputLabel } from './InputLabel';
import { Text } from './Text';
import {
  type AccessabilityLabel,
  type InputSizeVariant,
  inputVariantSize,
  isLabelAccessabilityLabel,
} from './utils';

export type InputProps = Omit<TextInputPropsRN, 'onChange' | 'onChangeText'> & {
  label: AccessabilityLabel;
  size?: InputSizeVariant;
  error?: string | ReactNode;
  value?: string;
  onChange: (val: string) => void;
};

// This is just input representation without any controlled state
export const Input = ({
  label,
  size = 'md',
  error,
  className,
  onChange,
  value = '',
  ...rest
}: InputProps) => {
  const classes = [
    'border px-4 py-2 rounded bg-surface border-secondary text-secondary placeholder:text-secondaryLow',
    inputVariantSize[size],
  ];

  const handleChangeText = (val: string): void => {
    onChange(val);
  };

  return (
    <View className={className}>
      <InputLabel label={label} className="mb-1" />

      <TextInputRN
        className={classes.join(' ')}
        accessibilityLabel={
          isLabelAccessabilityLabel(label) ? label.accessabilityLabel : label
        }
        {...rest}
        value={value}
        onChangeText={handleChangeText}
      />

      {error && <Text className="mt-1">{error}</Text>}
    </View>
  );
};
