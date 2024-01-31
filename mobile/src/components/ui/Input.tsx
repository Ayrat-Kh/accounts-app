import type { ReactNode } from 'react';
import {
  type TextInputProps as TextInputPropsRN,
  TextInput as TextInputRN,
  View,
} from 'react-native';

import { InputLabel } from './InputLabel';
import { Text } from './Text';
import { type AccessabilityLabel, isLabelAccessabilityLabel } from './utils';

export type InputProps = Omit<TextInputPropsRN, 'onChange' | 'onChangeText'> & {
  label: AccessabilityLabel;
  error?: string | ReactNode;
  isNumeric?: boolean;
  isDecimal?: boolean;
  onChange: (val: string | number) => void;
};

// This is just input representation without any controlled state
export const Input = ({
  label,
  error,
  isDecimal,
  isNumeric,
  className,
  onChange,
  ...rest
}: InputProps) => {
  const handleChangeText = (val: string): void => {
    if (isNumeric) {
      if (isDecimal) {
        onChange(parseFloat(val));
      } else {
        onChange(parseInt(val));
      }
    } else {
      onChange(val);
    }
  };

  return (
    <View className={className}>
      <InputLabel label={label} className="mb-1" />

      <TextInputRN
        className="border px-4 py-2 rounded  bg-primary border-primary h-[46]"
        accessibilityLabel={
          isLabelAccessabilityLabel(label) ? label.accessabilityLabel : label
        }
        {...rest}
        keyboardType={
          isNumeric ? (isDecimal ? 'decimal-pad' : 'numeric') : 'default'
        }
        onChangeText={handleChangeText}
      />

      {error && <Text className="mt-1">{error}</Text>}
    </View>
  );
};
