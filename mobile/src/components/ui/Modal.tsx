import type { FC, PropsWithChildren, ReactNode } from 'react';
import { Modal as ModalRN, View } from 'react-native';

import { IconCloseOutlinedNoBorder } from '~/assets/icons';
import { Button } from '~/components/ui/Button';
import { Text } from '~/components/ui/Text';

type ModalProps = PropsWithChildren<{
  header: string | ReactNode;
  isVisible: boolean;
  shouldCloseWindow?: () => boolean;
  onClose: VoidFunction;
}>;

export const Modal: FC<ModalProps> = ({
  children,
  header,
  isVisible,
  shouldCloseWindow = undefined,
  onClose,
}: ModalProps) => {
  return (
    <ModalRN
      visible={isVisible}
      animationType="slide"
      presentationStyle="formSheet"
      onRequestClose={() => {
        if (shouldCloseWindow && !shouldCloseWindow()) {
          return;
        }

        onClose();
      }}
    >
      <View className="flex-row justify-between items-center border-b-4 px-4 pt-4 pb-3 border-app-primary-600 bg-app-primary-300">
        {typeof header === 'string' ? (
          <Text variant="h3" kind="bold" color="secondary">
            {header}
          </Text>
        ) : (
          header
        )}
        <Button variant="ghost" onPress={onClose}>
          <IconCloseOutlinedNoBorder size={20} color="secondary" />
        </Button>
      </View>
      <View className="px-4 pt-3">{children}</View>
    </ModalRN>
  );
};
