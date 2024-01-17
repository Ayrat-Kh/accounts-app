import type { Config } from 'tailwindcss';

import { appColors } from '~/ui/colors';

module.exports = {
  content: ['./App.tsx', './src/**/*.{js,jsx,ts,tsx}'],
  theme: {
    colors: appColors,
  },
} satisfies Config;
