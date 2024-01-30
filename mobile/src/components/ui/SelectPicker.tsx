import { FlashList } from '@shopify/flash-list';
import { type ReactNode, useState } from 'react';
import { View } from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';

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
import { type AccessabilityLabel, getAccessabilityLabelNode } from './utils';

type SelectPickerModalProps<
  TData,
  TValue extends string | number,
  TMultiple extends boolean,
> = (TMultiple extends true
  ? {
      isMultiple: true;
      selected?: SelectPickerItemOption<TData, TValue>[];
      onChange: (value: SelectPickerItemOption<TData, TValue>[]) => void;
    }
  : {
      isMultiple: false;
      selected?: SelectPickerItemOption<TData, TValue>;
      onChange: (value: SelectPickerItemOption<TData, TValue>) => void;
    }) & {
  options: SelectPickerItemOption<TData, TValue>[];
  label: AccessabilityLabel;
  isVisible: boolean;
  close: VoidFunction;
};

const SelectPickerModal = <
  TData,
  TValue extends string | number,
  TMultiple extends boolean,
>({
  isMultiple,
  options,
  label,
  isVisible,
  selected,
  close,
  onChange,
}: SelectPickerModalProps<TData, TValue, TMultiple>) => {
  const [selectedState, setSelectedState] = useState<typeof selected>(selected);
  const { bottom } = useSafeAreaInsets();

  const extraData: SelectPickerItemExtraData<TData, TValue> = {
    selected: selectedState,
    onChange: (value: SelectPickerItemOption<TData, TValue>) => {
      if (isMultiple) {
        const filteredData = [
          ...(selected?.filter((x) => x.value !== value.value) ?? []),
        ];

        if (value) {
          filteredData.push(value);
        }

        setSelectedState(filteredData);
      } else {
        setSelectedState(value);
      }
    },
  };

  const handleApply = () => {
    if (isMultiple) {
      onChange(selectedState as SelectPickerItemOption<TData, TValue>[]);
    } else {
      onChange(selectedState as SelectPickerItemOption<TData, TValue>);
    }
    close();
  };

  return (
    <Modal
      isVisible={isVisible}
      header={getAccessabilityLabelNode(label)}
      onClose={close}
    >
      <FlashList
        estimatedItemSize={60}
        extraData={extraData}
        data={options}
        renderItem={SelectPickerItem<TData, TValue>}
        ItemSeparatorComponent={SelectPickerItemSeparator}
      />
      <View style={{ paddingBottom: bottom }}>
        <Button variant="primary" onPress={handleApply}>
          Apply
        </Button>
        <Button variant="secondary" className="mt-2" onPress={close}>
          Cancel
        </Button>
      </View>
    </Modal>
  );
};

export type SelectPickerProps<
  TData,
  TValue extends string | number,
  TMultiple extends boolean,
> = (TMultiple extends true
  ? {
      isMultiple: true;
      selected?: SelectPickerItemOption<TData, TValue>[];
      onChange: (value: SelectPickerItemOption<TData, TValue>[]) => void;
    }
  : {
      isMultiple: false;
      selected?: SelectPickerItemOption<TData, TValue>;
      onChange: (value: SelectPickerItemOption<TData, TValue>) => void;
    }) & {
  options: SelectPickerItemOption<TData, TValue>[];
  label: AccessabilityLabel;
  error?: string | ReactNode;
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

  const itemKey = isMultiple
    ? selected?.map((x) => x.value).join('')
    : selected?.value;

  const itemText =
    (isMultiple ? selected?.map((x) => x.label).join(', ') : selected?.label) ||
    'Select items';

  return (
    <View>
      <InputLabel label={label} className="mb-1" />
      <Button variant="input" align="left" onPress={show}>
        {itemText}
      </Button>
      {error && <Text className="mt-1">{error}</Text>}

      {/** weird... */}
      {isMultiple ? (
        <SelectPickerModal<TData, TValue, typeof isMultiple>
          key={itemKey}
          isMultiple={isMultiple}
          isVisible={isVisible}
          options={options}
          selected={selected}
          label={label}
          close={close}
          onChange={onChange}
        />
      ) : (
        <SelectPickerModal<TData, TValue, typeof isMultiple>
          key={itemKey}
          isMultiple={isMultiple}
          isVisible={isVisible}
          options={options}
          selected={selected}
          label={label}
          close={close}
          onChange={onChange}
        />
      )}
    </View>
  );
};
