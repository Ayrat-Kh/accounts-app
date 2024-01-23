import { Line, Svg } from 'react-native-svg';

import { type BaseSettingProps, getIconStrokeColor } from './utils';

export const IconCloseOutlinedNoBorder: React.FC<BaseSettingProps> = ({
  size = 50,
  color = 'primary',
}: BaseSettingProps) => {
  return (
    <Svg width={size} height={size} viewBox="0 0 256 256">
      <Line
        fill="none"
        stroke={getIconStrokeColor(color)}
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
        stroke={getIconStrokeColor(color)}
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
