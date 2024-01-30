import { ListRenderItemInfo } from '@shopify/flash-list';

import { Button } from './Button';
import { Checkbox } from './Checkbox';

export type SelectPickerItemOption<TData, TValue extends string | number> = {
  data: TData;
  value: TValue;
  label: string;
};

export type SelectPickerItemExtraData<TData, TValue extends string | number> = {
  selected?:
    | SelectPickerItemOption<TData, TValue>[]
    | SelectPickerItemOption<TData, TValue>;
  onChange: (value: SelectPickerItemOption<TData, TValue>) => void;
};

export type SelectPickerItemProps<TData, TValue extends string | number> = {
  extraData?: SelectPickerItemExtraData<TData, TValue>;
} & Omit<
  ListRenderItemInfo<SelectPickerItemOption<TData, TValue>>,
  'extraData' | 'onChange'
>;

export const SelectPickerItem = <TData, TValue extends string | number>({
  extraData,
  item,
}: SelectPickerItemProps<TData, TValue>) => {
  const isSelected = Array.isArray(extraData?.selected)
    ? extraData.selected.some((x) => x.value === item.value)
    : extraData?.selected?.value === item.value;

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
};
