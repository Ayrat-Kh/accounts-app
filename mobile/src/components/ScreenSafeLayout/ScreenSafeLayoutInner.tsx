import type { FC, PropsWithChildren } from 'react';
import { View } from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';

type ScreenSafeLayoutInnerProps = PropsWithChildren;

export const ScreenSafeLayoutInner: FC<ScreenSafeLayoutInnerProps> = ({
  children,
}: ScreenSafeLayoutInnerProps) => {
  const { bottom, left, right, top } = useSafeAreaInsets();
  return (
    <View
      className="flex-1"
      style={{
        paddingTop: top,
        paddingLeft: left,
        paddingRight: right,
        paddingBottom: bottom,
      }}
    >
      {children}
    </View>
  );
};
