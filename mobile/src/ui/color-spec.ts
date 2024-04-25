import type { Config } from 'tailwindcss';

// .dark, .dark-theme {
//   --gray-1: #000001;
//   --gray-2: #0d1122;
//   --gray-3: #171d37;
//   --gray-4: #1d2447;
//   --gray-5: #242c54;
//   --gray-6: #2a3466;
//   --gray-7: #354080;
//   --gray-8: #4957aa;
//   --gray-9: #5566ba;
//   --gray-10: #6374c5;
//   --gray-11: #a0b0ef;
//   --gray-12: #e8edff;

//   --gray-a1: #0000ff01;
//   --gray-a2: #6280ff22;
//   --gray-a3: #6b87ff37;
//   --gray-a4: #6982ff47;
//   --gray-a5: #6e86ff54;
//   --gray-a6: #6982ff66;
//   --gray-a7: #6a80ff80;
//   --gray-a8: #6e83ffaa;
//   --gray-a9: #758cffba;
//   --gray-a10: #8096ffc5;
//   --gray-a11: #abbcffef;
//   --gray-a12: #e8edff;

//   --gray-contrast: #FFFFFF;
//   --gray-surface: ;
//   --gray-indicator: #5566ba;
//   --gray-track: #5566ba;
// }

const appColors = {
  transparent: '#00000000',
  surface: '#0000002D',
  white: '#ffffff',
  purple: {
    // bg
    100: '#000000',
    200: '#170d1c',
    // interactive components
    300: '#2d1637',
    400: '#3e1d4c',
    500: '#4b255a',
    // borders and separators
    600: '#583069',
    700: '#6d3e80',
    800: '#8d52a6',
    // solid colors
    900: '#862aa7',
    1000: '#761496',
    // accessible text
    1100: '#e195ff',
    1200: '#efd7fa',
  },
  gray: {
    // bg
    100: '#000001',
    200: '#0d1122',
    // interactive components
    300: '#171d37',
    400: '#1d2447',
    500: '#242c54',
    // borders and separators
    600: '#2a3466',
    700: '#354080',
    800: '#4957aa',
    // solid colors
    900: '#5566ba',
    1000: '#6374c5',
    // accessible text
    1100: '#a0b0ef',
    1200: '#e8edff',
  },
} as const;

export const appTextColors = {
  primaryLow: appColors.purple[1100],
  primary: appColors.purple[1200],
  secondaryLow: appColors.gray[1100],
  secondary: appColors.gray[1200],
} as const satisfies Required<Config>['theme']['colors'];

export const appBgColors = {
  primaryLow: appColors.purple[100],
  primary: appColors.purple[200],
  secondaryLow: appColors.gray[100],
  secondary: appColors.gray[200],
  compPrimary: appColors.purple[300],
  compSecondary: appColors.gray[300],
  primarySolid: appColors.purple[900],
  secondarySolid: appColors.gray[900],
  surface: appColors.surface,
} as const satisfies Required<Config>['theme']['colors'];

export const appBorderColors = {
  primary: appColors.purple[600],
  secondary: appColors.gray[600],
} as const satisfies Required<Config>['theme']['colors'];

export const appFillColors = {
  primary: appColors.purple[800],
  secondary: appColors.gray[1000],
} as const satisfies Required<Config>['theme']['colors'];

export const appStrokeColors = {
  primary: appColors.purple[800],
  secondary: appColors.gray[1000],
} as const satisfies Required<Config>['theme']['colors'];
