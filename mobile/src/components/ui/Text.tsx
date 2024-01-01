import { WhiteText } from './Text.styles';

export const Text: React.FC<React.PropsWithChildren> = ({ children }) => {
  return <WhiteText>{children}</WhiteText>;
};
