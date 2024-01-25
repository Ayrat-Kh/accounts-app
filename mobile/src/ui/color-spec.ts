import type { Config } from 'tailwindcss';

const appColors = {
  transparent: '#00000000',
  white: '#ffffff',
  green: {
    // bg
    100: '#FBFEFB',
    200: '#F5FBF5',
    // interactive components
    300: '#E9F6E9',
    400: '#DAF1DB',
    500: '#C9E8CA',
    // borders and separators
    600: '#B2DDB5',
    700: '#94CE9A',
    800: '#65BA74',
    // solid colors
    900: '#46A758',
    1000: '#3E9B4F',
    // accessible text
    1100: '#2A7E3B',
    1200: '#203C25',
  },
  gray: {
    // bg
    100: '#FCFCFC',
    200: '#F9F9F9',
    // interactive components
    300: '#F0F0F0',
    400: '#E8E8E8',
    500: '#E0E0E0',
    // borders and separators
    600: '#D9D9D9',
    700: '#CECECE',
    800: '#BBBBBB',
    // solid colors
    900: '#8D8D8D',
    1000: '#838383',
    // accessible text
    1100: '#646464',
    1200: '#202020',
  },
} as const;

export const appTextColors = {
  primaryLow: appColors.green[1100],
  primary: appColors.green[1200],
  secondaryLow: appColors.gray[1100],
  secondary: appColors.gray[1200],
} as const satisfies Required<Config>['theme']['colors'];

export const appBgColors = {
  primaryLow: appColors.green[100],
  primary: appColors.green[200],
  secondaryLow: appColors.gray[100],
  secondary: appColors.gray[200],
  compPrimary: appColors.green[300],
  compSecondary: appColors.gray[300],
  primarySolid: appColors.green[900],
  secondarySolid: appColors.gray[900],
} as const satisfies Required<Config>['theme']['colors'];

export const appBorderColors = {
  primary: appColors.green[600],
  secondary: appColors.gray[600],
} as const satisfies Required<Config>['theme']['colors'];

export const appFillColors = {
  primary: appColors.green[800],
  secondary: appColors.gray[1000],
} as const satisfies Required<Config>['theme']['colors'];

export const appStrokeColors = {
  primary: appColors.green[800],
  secondary: appColors.gray[1000],
} as const satisfies Required<Config>['theme']['colors'];
