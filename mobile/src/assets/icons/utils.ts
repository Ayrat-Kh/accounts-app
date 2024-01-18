import { type SvgProps } from 'react-native-svg';

export type FillColor = 'primary' | 'secondary';

export type BaseSettingProps = Omit<SvgProps, 'width' | 'height'> & {
  color: 'primary' | 'secondary';
  size: number;
};

export const getIconFillColor = (color: FillColor) => {
  switch (color) {
    case 'secondary': {
      return 'fill-app-secondary-1000';
    }
    case 'primary':
    default:
      return 'fill-app-primary-800';
  }
};

export const getIconStrokeColor = (color: FillColor) => {
  switch (color) {
    case 'secondary': {
      return 'stroke-app-secondary-1000';
    }
    case 'primary':
    default:
      return 'stroke-app-primary-800';
  }
};
