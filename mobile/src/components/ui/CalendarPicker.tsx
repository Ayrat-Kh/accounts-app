import { type ReactNode, useState } from 'react';
import { View } from 'react-native';
import { Calendar, type DateData } from 'react-native-calendars';
import { useSafeAreaInsets } from 'react-native-safe-area-context';

import { useBgColor, useBorderColor, useTextColor } from '~/ui';
import { formatISODate } from '~/utils';

import { Button } from './Button';
import { InputLabel } from './InputLabel';
import { Modal } from './Modal';
import { useModal } from './Modal.hooks';
import { Text } from './Text';
import { type AccessabilityLabel, getAccessabilityLabelNode } from './utils';

type CalendarPickerModalProps = {
  selectedDate?: string;
  label: AccessabilityLabel;
  isVisible: boolean;
  onChange: (newDateValue: string) => void;
  close: VoidFunction;
};

const CalendarPickerModal = ({
  label,
  isVisible,
  selectedDate,
  close,
  onChange,
}: CalendarPickerModalProps) => {
  const selectedTextColor = useTextColor('primary');
  const selectedTextBgColor = useBgColor('primarySolid');

  const textColor = useTextColor('primaryLow');

  const compBgColor = useBgColor('compPrimary');
  const borderColor = useBorderColor('primary');
  const [selectedState, setSelectedState] =
    useState<typeof selectedDate>(selectedDate);
  const { bottom } = useSafeAreaInsets();

  const selectedDateString = formatISODate(selectedState);

  const handleDayPress = (day: DateData) => {
    setSelectedState(day.dateString);
  };

  const handleApply = () => {
    if (selectedState) {
      onChange(selectedState);
    }

    close();
  };

  return (
    <Modal
      isVisible={isVisible}
      header={getAccessabilityLabelNode(label)}
      onClose={close}
    >
      <View className="flex-1 justify-between ">
        <Calendar
          style={{
            borderRadius: 5,
            borderWidth: 1,
            borderColor,
          }}
          theme={{
            backgroundColor: compBgColor,
            calendarBackground: compBgColor,
            todayTextColor: selectedTextColor,
            dayTextColor: textColor,
            arrowColor: textColor,
          }}
          markedDates={
            selectedDateString
              ? {
                  [selectedDateString]: {
                    selected: true,
                    selectedTextColor: selectedTextColor,
                    selectedColor: selectedTextBgColor,
                  },
                }
              : undefined
          }
          current={selectedState}
          onDayPress={handleDayPress}
        />

        <View style={{ paddingBottom: bottom }} className="mt-4">
          <Button variant="primary" onPress={handleApply}>
            Apply
          </Button>
          <Button variant="secondary" className="mt-2" onPress={close}>
            Cancel
          </Button>
        </View>
      </View>
    </Modal>
  );
};

export type CalendarPickerProps = Omit<
  CalendarPickerModalProps,
  'isVisible' | 'close'
> & {
  error?: string | ReactNode;
  className?: string;
  placeholder?: string;
};

export const CalendarPicker = ({
  error,
  label,
  selectedDate,
  className,
  placeholder,
  onChange,
}: CalendarPickerProps) => {
  const { isVisible, show, close } = useModal();

  const itemText = selectedDate || (placeholder ?? 'Select date...');

  return (
    <View className={className}>
      <InputLabel label={label} className="mb-1" />
      <Button variant="input" align="left" onPress={show}>
        {itemText}
      </Button>
      {error && <Text className="mt-1">{error}</Text>}

      <CalendarPickerModal
        key={selectedDate}
        isVisible={isVisible}
        selectedDate={selectedDate}
        label={label}
        close={close}
        onChange={onChange}
      />
    </View>
  );
};
