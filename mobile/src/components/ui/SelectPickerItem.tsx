import { ListRenderItemInfo } from '@shopify/flash-list';

import { Button } from './Button';
import { Checkbox } from './Checkbox';

export type SelectValue<
  TValue,
  TMultiple extends boolean,
> = TMultiple extends true ? TValue[] : TValue | null;

export type SelectPickerItemOption<TValue> = {
  value: TValue;
};

export type SelectPickerLabelItemOption<TValue> =
  SelectPickerItemOption<TValue> & {
    label: string;
  };

export type SelectPickerItemExtraData<TValue, TMultiple extends boolean> = {
  selected: SelectValue<TValue, TMultiple> | null;
  onChange: (value: SelectPickerItemOption<TValue>) => void;
};

export type SelectPickerItemProps<TValue, TMultiple extends boolean> = {
  extraData?: SelectPickerItemExtraData<TValue, TMultiple>;
} & Omit<
  ListRenderItemInfo<SelectPickerLabelItemOption<TValue>>,
  'extraData' | 'onChange'
>;

export function SelectPickerItem<TValue, TMultiple extends boolean>({
  extraData,
  item,
}: SelectPickerItemProps<TValue, TMultiple>) {
  const isSelected = Array.isArray(extraData?.selected)
    ? extraData.selected.some((x) => x.value === item.value)
    : extraData?.selected === item.value;

  const handlePress = () => {
    extraData?.onChange(item);
  };

  return (
    <Button align="left" variant="ghost" onPress={handlePress}>
      <Checkbox
        label={item.label}
        isChecked={isSelected}
        className="my-2"
        onChange={handlePress}
      />
    </Button>
  );
}
