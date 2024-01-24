import React from 'react';
import {
  type TextInputProps as TextInputPropsRN,
  TextInput as TextInputRN,
  View,
} from 'react-native';

import { Text } from './Text';

type AccessebilityLabelObject = {
  component: React.ReactNode;
  accessebilityLabel: string;
};

export type AccessebilityLabel = AccessebilityLabelObject | string;

export type InputProps<TLabel extends AccessebilityLabel> = TextInputPropsRN & {
  label: TLabel;
  error?: string | React.ReactNode;
};

const isLabelAccessebilityLabel = (
  obj: AccessebilityLabel,
): obj is AccessebilityLabelObject => {
  return typeof obj === 'object';
};

// This is just input representation without any controlled state
export const Input = <TLabel extends AccessebilityLabel>({
  label,
  error,
  ...rest
}: InputProps<TLabel>) => {
  return (
    <View>
      {isLabelAccessebilityLabel(label) ? (
        label.component
      ) : (
        <Text variant="base1" color="primary-low" className="mb-1">
          {label}
        </Text>
      )}

      <TextInputRN
        className="border px-4 py-2 rounded bg-app-primary-300 border-app-primary-600"
        accessibilityLabel={
          isLabelAccessebilityLabel(label) ? label.accessebilityLabel : label
        }
        {...rest}
      />
      {error && <Text className="mt-1">{error}</Text>}
    </View>
  );
};
