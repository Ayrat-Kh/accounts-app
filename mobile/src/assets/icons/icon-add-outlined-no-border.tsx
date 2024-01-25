import type { FC } from 'react';
import { Line, Svg } from 'react-native-svg';

import { useGetIconStrokeColor } from '~/ui/hooks';

import type { BaseSettingProps } from './types';

export const IconAddOutlinedNoBorder: FC<BaseSettingProps> = ({
  size = 50,
  color = 'primary',
}: BaseSettingProps) => {
  const getIconStrokeColor = useGetIconStrokeColor();

  return (
    <Svg width={size} height={size} viewBox="0 0 256 256">
      <Line
        fill="none"
        stroke={getIconStrokeColor(color)}
        strokeLinecap="round"
        strokeLinejoin="round"
        strokeWidth="24"
        x1="40"
        x2="216"
        y1="128"
        y2="128"
      />
      <Line
        fill="none"
        stroke={getIconStrokeColor(color)}
        strokeLinecap="round"
        strokeLinejoin="round"
        strokeWidth="24"
        x1="128"
        x2="128"
        y1="40"
        y2="216"
      />
    </Svg>
  );
};
