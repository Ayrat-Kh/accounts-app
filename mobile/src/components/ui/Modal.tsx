import type { FC, PropsWithChildren, ReactNode } from 'react';
import {
  Modal as ModalRN,
  type ModalProps as ModalRNProps,
  View,
} from 'react-native';

import { IconCloseOutlinedNoBorder } from '~/assets/icons';
import { Button } from '~/components/ui/Button';
import { Text } from '~/components/ui/Text';

type ModalProps = PropsWithChildren<{
  header: string | ReactNode;
  isVisible: boolean;
  animationType?: ModalRNProps['animationType'];
  presentationStyle?: ModalRNProps['presentationStyle'];
  shouldCloseWindow?: () => boolean;
  onClose: VoidFunction;
}>;

export const Modal: FC<ModalProps> = ({
  children,
  header,
  isVisible,
  animationType = 'slide',
  presentationStyle = 'formSheet',
  shouldCloseWindow = undefined,
  onClose,
}: ModalProps) => {
  return (
    <ModalRN
      visible={isVisible}
      animationType={animationType}
      presentationStyle={presentationStyle}
      onRequestClose={() => {
        if (shouldCloseWindow && !shouldCloseWindow()) {
          return;
        }

        onClose();
      }}
    >
      <View className="flex-row justify-between items-center border-b-4 px-4 pt-4 pb-3 border-secondary bg-compSecondary">
        {typeof header === 'string' ? (
          <Text variant="h3" kind="bold" color="secondary">
            {header}
          </Text>
        ) : (
          header
        )}
        <Button
          variant="ghost"
          icon={<IconCloseOutlinedNoBorder size={20} />}
          onPress={onClose}
        />
      </View>
      <View className="px-4 pt-3 flex-1 bg-secondary">{children}</View>
    </ModalRN>
  );
};
