import { ButtonContainer } from './Button.styles';

type ButtonProps = React.PropsWithChildren;

export const Button: React.FC<ButtonProps> = ({ children }) => {
  return <ButtonContainer>{children}</ButtonContainer>;
};
