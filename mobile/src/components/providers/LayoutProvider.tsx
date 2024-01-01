import React from 'react';
import { View } from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';

type LayoutProviderProps = React.PropsWithChildren;

export const LayoutProvider: React.FC<LayoutProviderProps> = ({
  children,
}: LayoutProviderProps) => {
  const { bottom, left, right, top } = useSafeAreaInsets();
  return (
    <View
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
