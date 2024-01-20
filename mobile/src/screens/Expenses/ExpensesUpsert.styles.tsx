import { type PropsWithChildren } from 'react';
import { View } from 'react-native';

export const ExpensesUpsertContainer = ({ children }: PropsWithChildren) => {
  return (
    <View className="flex-1 items-center justify-center bg-app-primary-100">
      {children}
    </View>
  );
};
