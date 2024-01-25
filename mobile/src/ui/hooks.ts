import {
  appBgColors,
  appFillColors,
  appStrokeColors,
  appTextColors,
} from './color-spec';

export type FillColor = 'primary' | 'secondary';

// later will add dark mode
export const useGetIconFillColor = () => {
  return (color: FillColor) => {
    switch (color) {
      case 'secondary': {
        return appFillColors.secondary;
      }
      case 'primary':
      default:
        return appFillColors.primary;
    }
  };
};

export const useGetIconStrokeColor = () => {
  return (color: FillColor) => {
    switch (color) {
      case 'secondary': {
        return appStrokeColors.secondary;
      }
      case 'primary':
      default:
        return appStrokeColors.primary;
    }
  };
};

// later will be useb for the dark theme
export const useBgColor = (color: keyof typeof appBgColors) => {
  return appBgColors[color];
};

// later will be useb for the dark theme
export const useTextColor = (color: keyof typeof appTextColors) => {
  return appTextColors[color];
};
