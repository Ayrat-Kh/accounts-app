import { TouchableOpacity } from "react-native";

type ButtonProps = React.PropsWithChildren<{}>;

export const Button: React.FC<ButtonProps> = ({ children }) => {
  return <TouchableOpacity>{children}</TouchableOpacity>;
};
