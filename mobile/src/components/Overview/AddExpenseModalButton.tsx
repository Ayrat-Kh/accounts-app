import { View } from 'react-native';

import { IconAddOutlinedNoBorder } from '~/assets/icons';
import { Button, Modal, useModal } from '~/components/ui';

import { CreateExpense } from './components/CreateExpense';

export const AddExpenseModalButton = () => {
  const { isVisible, close, show } = useModal();

  return (
    <>
      <View className="top-[-28] h-[56] max-w-[56] flex-1 justify-center items-center rounded-full bg-app-primary-200">
        <Button variant="primary" rounded maxWidth onPress={show}>
          <IconAddOutlinedNoBorder color="primary" size={40} />
        </Button>
      </View>
      {/** Create expense modal by clicking + button **/}
      <Modal header="Add expense" isVisible={isVisible} onClose={close}>
        <CreateExpense />
      </Modal>
    </>
  );
};
