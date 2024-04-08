import { Path, Svg } from 'react-native-svg';

import { useStrokeColor } from '~/ui/hooks';

import type { BaseSettingProps } from './types';

export const IconLocation: React.FC<BaseSettingProps> = ({
  size = 50,
  color = 'primary',
  ...rest
}: BaseSettingProps) => {
  const strokeColor = useStrokeColor(color);

  return (
    <Svg width={size} height={size} viewBox="0 0 24 24" {...rest}>
      <Path
        fill={strokeColor}
        d="M12 21C15.5 17.4 19 14.1764 19 10.2C19 6.22355 15.866 3 12 3C8.13401 3 5 6.22355 5 10.2C5 14.1764 8.5 17.4 12 21Z"
        strokeLinecap="round"
        strokeWidth={2}
        strokeLinejoin="round"
      />
    </Svg>
  );
};
