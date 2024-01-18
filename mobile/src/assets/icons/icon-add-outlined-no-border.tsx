import { styled } from 'nativewind';
import { Line, Svg } from 'react-native-svg';

import { type BaseSettingProps, getIconStrokeColor } from './utils';

const StyledLine = styled(Line, { classProps: ['stroke'] });

export const IconAddOutlinedNoBorder: React.FC<BaseSettingProps> = ({
  size = 50,
  color = 'primary',
}: BaseSettingProps) => {
  return (
    <Svg width={size} height={size} viewBox="0 0 256 256">
      <StyledLine
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
      <StyledLine
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
