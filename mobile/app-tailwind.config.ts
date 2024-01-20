import type { Config } from 'tailwindcss';

import { appColors } from '~/components/ui/colors';

export default {
  content: ['./App.tsx', './src/**/*.{js,jsx,ts,tsx}'],
  theme: {
    colors: appColors,
    fill: appColors,
    stroke: appColors,
  },
} satisfies Config;
