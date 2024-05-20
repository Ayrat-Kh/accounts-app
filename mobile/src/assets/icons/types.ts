import { type SvgProps } from 'react-native-svg';

export type IconColor = 'primary' | 'secondary';

export type BaseSettingProps = Omit<SvgProps, 'width' | 'height'> & {
  color?: IconColor;
  size: number;
};
