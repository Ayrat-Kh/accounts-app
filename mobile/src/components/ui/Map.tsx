import type { FC } from 'react';
import { View } from 'react-native';
import MapView, { type MapViewProps } from 'react-native-maps';

type MapProps = MapViewProps & {};

export const Map: FC<MapProps> = () => {
  return (
    <View>
      <MapView style={{ width: '100%', height: '100%' }} />
    </View>
  );
};
