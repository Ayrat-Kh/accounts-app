import type { FC, PropsWithChildren } from 'react';
import { View } from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';

type LayoutProviderProps = PropsWithChildren;

export const LayoutProvider: FC<LayoutProviderProps> = ({
  children,
}: LayoutProviderProps) => {
  const { bottom, left, right, top } = useSafeAreaInsets();
  return (
    <View
      className="flex-1 bg-primary"
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
