import { type TouchableOpacityProps } from 'react-native';

import { ButtonContainer } from './Button.styles';

type ButtonProps = React.PropsWithChildren<TouchableOpacityProps>;

export const Button: React.FC<ButtonProps> = ({ children, ...rest }) => {
  return <ButtonContainer {...rest}>{children}</ButtonContainer>;
};