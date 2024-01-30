import { type SvgProps } from 'react-native-svg';

export type BaseSettingProps = Omit<SvgProps, 'width' | 'height'> & {
  color: 'primary' | 'secondary';
  size: number;
};
