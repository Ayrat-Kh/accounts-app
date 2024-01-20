import { type PropsWithChildren } from 'react';
import { TouchableOpacity } from 'react-native';

export const ButtonContainer = ({ children }: PropsWithChildren) => {
  return (
    <TouchableOpacity className="'py-2 px-4 border rounded-md flex-1 justify-items-center items-center flex-row'">
      {children}
    </TouchableOpacity>
  );
};
