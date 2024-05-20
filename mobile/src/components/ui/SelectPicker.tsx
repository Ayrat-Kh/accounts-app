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
  type SelectPickerLabelItemOption,
  type SelectValue,
} from './SelectPickerItem';
import { SelectPickerItemSeparator } from './SelectPickerItemSeparator';
import { Text } from './Text';
import {
  type AccessabilityLabel,
  type InputSizeVariant,
  getAccessabilityLabelNode,
} from './utils';

type SelectPickerModalProps<TValue, TMultiple extends boolean> = {
  selected?: SelectValue<TValue, TMultiple>;
  isMultiple: TMultiple;
  options: SelectPickerLabelItemOption<TValue>[];
  label: AccessabilityLabel;
  isVisible: boolean;
  close: VoidFunction;
  onChange: (value: SelectValue<TValue, TMultiple>) => void;
};

function SelectPickerModal<TValue, TMultiple extends boolean>({
  isMultiple,
  options,
  label,
  isVisible,
  selected,
  close,
  onChange,
}: SelectPickerModalProps<TValue, TMultiple>) {
  const [selectedState, setSelectedState] = useState<SelectValue<
    TValue,
    TMultiple
  > | null>(selected ?? null);
  const { bottom } = useSafeAreaInsets();

  const extraData: SelectPickerItemExtraData<TValue, TMultiple> = {
    selected: selectedState,
    onChange: (value: SelectPickerItemOption<TValue>) => {
      // handle multiple cases
      if (isMultiple) {
        let filteredData: TValue[] = [];

        if (Array.isArray(selectedState)) {
          const a = selectedState as TValue[];
          filteredData = a.filter((x) => x !== value.value);
        }

        if (value) {
          filteredData.push(value.value);
        }

        setSelectedState(filteredData as SelectValue<TValue, TMultiple>);

        return;
      }

      // handle single case
      if (!isMultiple) {
        setSelectedState(value.value as SelectValue<TValue, TMultiple>);
      }
    },
  };

  const handleApply = () => {
    onChange(selectedState as SelectValue<TValue, TMultiple>);

    if (!isMultiple) {
      close();
    }
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
        renderItem={SelectPickerItem<TValue, TMultiple>}
        ItemSeparatorComponent={SelectPickerItemSeparator}
      />
      <View style={{ paddingBottom: bottom + 10 }}>
        <Button variant="primary" onPress={handleApply}>
          Apply
        </Button>
        <Button variant="secondary" className="mt-2" onPress={close}>
          Cancel
        </Button>
      </View>
    </Modal>
  );
}

export type SelectPickerProps<TValue, TMultiple extends boolean> = Omit<
  SelectPickerModalProps<TValue, TMultiple>,
  'close' | 'isVisible'
> & {
  size?: InputSizeVariant;
  options: SelectPickerLabelItemOption<TValue>[];
  label: AccessabilityLabel;
  error?: string | ReactNode;
  className?: string;
  placeholder?: string;
};

export function SelectPicker<TValue, TMultiple extends boolean>({
  size = 'md',
  isMultiple,
  error,
  label,
  options,
  selected,
  className,
  placeholder,
  onChange,
}: SelectPickerProps<TValue, TMultiple>) {
  const { isVisible, show, close } = useModal();

  let selectedText = '';
  let itemKey = '';

  if (Array.isArray(selected)) {
    itemKey = selected?.join('') ?? '';

    selectedText =
      options.filter((x) => selected.includes(x.value)).join(', ') ??
      placeholder ??
      'Select items...';
  } else {
    itemKey = `${selected}` ?? '';

    selectedText =
      options.find((x) => selected === x.value)?.label ??
      placeholder ??
      'Selected items...';
  }

  return (
    <View className={className}>
      <InputLabel label={label} className="mb-1" />
      <Button size={size} variant="input" align="left" onPress={show}>
        {selectedText}
      </Button>
      {error && <Text className="mt-1">{error}</Text>}

      <SelectPickerModal<TValue, TMultiple>
        key={itemKey}
        isMultiple={isMultiple}
        isVisible={isVisible}
        options={options}
        selected={selected}
        label={label}
        close={close}
        onChange={onChange}
      />
    </View>
  );
}
