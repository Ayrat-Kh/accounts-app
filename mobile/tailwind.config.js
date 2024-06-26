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
  corePlugin: {
    backgroundOpacity: true,
  },
  theme: {
    extend: {
      textColor: appTextColors,
      borderColor: appBorderColors,
      backgroundColor: appBgColors,
      fill: appFillColors,
      stroke: appStrokeColors,
      fontFamily: {
        app: ['Inter'],
      },
    },
  },
  future: {
    hoverOnlyWhenSupported: true,
  },
  plugins: [],
};
