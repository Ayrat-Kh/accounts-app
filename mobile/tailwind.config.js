import {
  appBgColors,
  appBorderColors,
  appFillColors,
  appStrokeColors,
  appTextColors,
} from './src/ui';

/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ['./App.tsx', './src/**/*.{js,jsx,ts,tsx}'],
  presets: [require('nativewind/preset')],
  theme: {
    extend: {
      textColor: appTextColors,
      borderColor: appBorderColors,
      backgroundColor: appBgColors,
      fill: appFillColors,
      stroke: appStrokeColors,
    },
  },
  future: {
    hoverOnlyWhenSupported: true,
  },
  plugins: [],
};
