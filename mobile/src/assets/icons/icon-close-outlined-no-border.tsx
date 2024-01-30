import type { FC } from 'react';
import { Line, Svg } from 'react-native-svg';

import { useStrokeColor } from '~/ui/hooks';

import type { BaseSettingProps } from './types';

export const IconCloseOutlinedNoBorder: FC<BaseSettingProps> = ({
  size = 50,
  color = 'primary',
}: BaseSettingProps) => {
  const strokeColor = useStrokeColor(color);

  return (
    <Svg width={size} height={size} viewBox="0 0 256 256">
      <Line
        fill="none"
        stroke={strokeColor}
        strokeLinecap="round"
        strokeLinejoin="round"
        strokeWidth="50"
        x1="40"
        y1="40"
        x2="216"
        y2="216"
      />
      <Line
        fill="none"
        stroke={strokeColor}
        strokeLinecap="round"
        strokeLinejoin="round"
        strokeWidth="50"
        x1="216"
        y1="40"
        x2="40"
        y2="216"
      />
    </Svg>
  );
};
