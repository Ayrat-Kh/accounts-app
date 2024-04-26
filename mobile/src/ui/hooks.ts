import {
  AppBgColorKey,
  appBgColors,
  appBorderColors,
  appFillColors,
  appStrokeColors,
  appTextColors,
} from './color-spec';

export type FillColor = 'primary' | 'secondary';

// later will add dark mode
export const useFillColor = (color: FillColor) => {
  switch (color) {
    case 'secondary': {
      return appFillColors.secondary;
    }
    case 'primary':
    default:
      return appFillColors.primary;
  }
};

export const useStrokeColor = (color: FillColor) => {
  switch (color) {
    case 'secondary': {
      return appStrokeColors.secondary;
    }
    case 'primary':
    default:
      return appStrokeColors.primary;
  }
};

// later will be used for the dark theme
export const useBgColor = (color: AppBgColorKey) => {
  return appBgColors[color];
};

export const useBorderColor = (color: keyof typeof appBorderColors) => {
  return appBorderColors[color];
};

// later will be used for the dark theme
export const useTextColor = (color: keyof typeof appTextColors) => {
  return appTextColors[color];
};
