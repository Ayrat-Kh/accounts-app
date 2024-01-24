import React from 'react';
import { FlatList, View } from 'react-native';

import { Input } from './Input';
import { InputLabel } from './InputLabel';
import { Modal } from './Modal';
import { useModal } from './Modal.hooks';
import { Text } from './Text';
import { type AccessebilityLabel, getAccessebilityLabelNode } from './utils';

export type TOption<TItemValue> = { value: TItemValue; label: string };

export type SelectPickerProps<
  TItemValue extends string | number | Record<string, unknown>,
> = {
  label: AccessebilityLabel;
  options: { value: TItemValue; label: string }[];
  selected?: TItemValue;
  error?: string | React.ReactNode;
  onChange: (value: TItemValue) => void;
};

export const SelectPicker = <
  TItemValue extends string | number | Record<string, unknown>,
>({
  error,
  label,
  options,
  selected,
  onChange,
}: SelectPickerProps<TItemValue>) => {
  const { show, isVisible } = useModal();

  return (
    <View>
      <InputLabel label={label} className="mb-1" />
      <Input />
      {error && <Text className="mt-1">{error}</Text>}

      <Modal isVisible={isVisible} header={getAccessebilityLabelNode(label)}>
        <FlashList
          data={options}
          ItemSeparatorComponent={() => <View className="border" />}
        />
      </Modal>
    </View>
  );
};
