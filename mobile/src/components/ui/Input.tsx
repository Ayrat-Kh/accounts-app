import {
  type TextInputProps as TextInputPropsRN,
  TextInput as TextInputRN,
} from 'react-native';

type TextInputProps = TextInputPropsRN;

export const TextInput: React.FC<TextInputProps> = (props: TextInputProps) => {
  return <TextInputRN {...props} />;
};
