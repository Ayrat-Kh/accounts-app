import { FlashList } from '@shopify/flash-list';
import React from 'react';
import { View } from 'react-native';

import { Button } from './Button';
import { InputLabel } from './InputLabel';
import { Modal } from './Modal';
import { useModal } from './Modal.hooks';
import {
  SelectPickerItem,
  type SelectPickerItemExtraData,
  type SelectPickerItemOption,
} from './SelectPickerItem';
import { SelectPickerItemSeparator } from './SelectPickerItemSeparator';
import { Text } from './Text';
import { type AccessebilityLabel, getAccessebilityLabelNode } from './utils';

export type SelectPickerProps<
  TData,
  TValue extends string | number,
  TMultiple extends boolean,
> = (TMultiple extends true
  ? {
      isMultiple: true;
      selected: SelectPickerItemOption<TData, TValue>[];
      onChange: (value: SelectPickerItemOption<TData, TValue>[]) => void;
    }
  : {
      isMultiple: false;
      selected: SelectPickerItemOption<TData, TValue>;
      onChange: (
        value: SelectPickerItemOption<TData, TValue>,
        isChecked: boolean,
      ) => void;
    }) & {
  options: SelectPickerItemOption<TData, TValue>[];
  label: AccessebilityLabel;
  error?: string | React.ReactNode;
};

export const SelectPicker = <
  TData,
  TValue extends string | number,
  TMultiple extends boolean,
>({
  isMultiple,
  error,
  label,
  options,
  selected,
  onChange,
}: SelectPickerProps<TData, TValue, TMultiple>) => {
  const { isVisible, show, close } = useModal();

  const extraData: SelectPickerItemExtraData<TData, TValue> = {
    selected,
    onChange: (
      isChecked: boolean,
      value: SelectPickerItemOption<TData, TValue>,
    ) => {
      if (isMultiple) {
        const filteredData = [
          ...selected.filter((x) => x.value !== value.value),
        ];

        if (isChecked) {
          onChange([...filteredData, value]);
        } else {
          onChange(filteredData);
        }
      } else {
        onChange(value, isChecked);
      }
    },
  };

  return (
    <View>
      <InputLabel label={label} className="mb-1" />
      <Button variant="primary" onPress={show} />
      {error && <Text className="mt-1">{error}</Text>}

      <Modal
        isVisible={isVisible}
        header={getAccessebilityLabelNode(label)}
        onClose={close}
      >
        <FlashList
          extraData={extraData}
          data={options}
          renderItem={SelectPickerItem<TData, TValue>}
          ItemSeparatorComponent={SelectPickerItemSeparator}
        />
      </Modal>
    </View>
  );
};
