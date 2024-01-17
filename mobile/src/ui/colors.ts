import type { Config } from 'tailwindcss';

export const appColors = {
  transparent: 'transparent',
  white: '#ffffff',
  ['app-primary']: {
    // bg
    100: '#FBFEFB',
    200: '#F5FBF5',
    // Interactive components
    300: '#E9F6E9',
    400: '#DAF1DB',
    500: '#C9E8CA',
    // Borders and separators
    600: '#B2DDB5',
    700: '#94CE9A',
    800: '#65BA74',
    // solid colors
    900: '#46A758',
    1000: '#3E9B4F',
    // Accessible text
    1100: '#2A7E3B',
    1200: '#203C25',
  },
  ['app-secondary']: {
    // bg
    100: '#FCFCFC',
    200: '#F9F9F9',
    // Interactive components
    300: '#F0F0F0',
    400: '#E8E8E8',
    500: '#E0E0E0',
    // Borders and separators
    600: '#D9D9D9',
    700: '#CECECE',
    800: '#BBBBBB',
    // solid colors
    900: '#8D8D8D',
    1000: '#838383',
    // Accessible text
    1100: '#646464',
    1200: '#202020',
  },
} satisfies Required<Config>['theme']['colors'];
