// import { styled } from 'nativewind';
import { Path, Svg, type SvgProps } from 'react-native-svg';

// figure out why it's not working?
// const StyledPath = styled(Path, { classProps: ['fill'] });
type IconGoogleProps = SvgProps & {
  color: 'primary' | 'secondary';
};

export const IconGoogle: React.FC<IconGoogleProps> = ({
  width = 50,
  height = 50,
  color = 'primary',
}: IconGoogleProps) => {
  const classNames = [];

  switch (color) {
    case 'primary': {
      classNames.push('color-app-primary-1200');
      break;
    }
    case 'secondary': {
      classNames.push('color-app-secondary-1200');
      break;
    }
  }

  return (
    <Svg x="0" y="0" width={width} height={height} viewBox="0 0 50 50">
      <Path
        fill="grey"
        d="M 25.996094 48 C 13.3125 48 2.992188 37.683594 2.992188 25 C 2.992188 12.316406 13.3125 2 25.996094 2 C 31.742188 2 37.242188 4.128906 41.488281 7.996094 L 42.261719 8.703125 L 34.675781 16.289063 L 33.972656 15.6875 C 31.746094 13.78125 28.914063 12.730469 25.996094 12.730469 C 19.230469 12.730469 13.722656 18.234375 13.722656 25 C 13.722656 31.765625 19.230469 37.269531 25.996094 37.269531 C 30.875 37.269531 34.730469 34.777344 36.546875 30.53125 L 24.996094 30.53125 L 24.996094 20.175781 L 47.546875 20.207031 L 47.714844 21 C 48.890625 26.582031 47.949219 34.792969 43.183594 40.667969 C 39.238281 45.53125 33.457031 48 25.996094 48 Z"
      />
    </Svg>
  );
};
