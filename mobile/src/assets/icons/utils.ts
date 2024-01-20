import { type SvgProps } from 'react-native-svg';

import { appColors } from '~/components/ui/colors';

export type FillColor = 'primary' | 'secondary';

export type BaseSettingProps = Omit<SvgProps, 'width' | 'height'> & {
  color: 'primary' | 'secondary';
  size: number;
};

export const getIconFillColor = (color: FillColor) => {
  switch (color) {
    case 'secondary': {
      return appColors['app-secondary'][1000];
    }
    case 'primary':
    default:
      return appColors['app-primary'][800];
  }
};

export const getIconStrokeColor = (color: FillColor) => {
  switch (color) {
    case 'secondary': {
      return appColors['app-secondary'][1000];
    }
    case 'primary':
    default:
      return appColors['app-primary'][800];
  }
};
