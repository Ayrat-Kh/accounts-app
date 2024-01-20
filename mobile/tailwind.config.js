import { appColors } from './src/components/ui/colors';

/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ['./App.tsx', './src/**/*.{js,jsx,ts,tsx}'],
  presets: [require('nativewind/preset')],
  theme: {
    extend: {
      colors: appColors,
      fill: appColors,
      stroke: appColors,
    },
  },
  future: {
    hoverOnlyWhenSupported: true,
  },
  plugins: [],
};
