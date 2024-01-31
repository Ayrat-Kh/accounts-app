import type { FC } from 'react';
import { StyleSheet, View } from 'react-native';
import MapView, { type MapViewProps } from 'react-native-maps';

type MapProps = MapViewProps & {
  className?: string;
};

export const Map: FC<MapProps> = ({ className }) => {
  const totalClassNames: (string | unknown)[] = ['rounded', className];

  return (
    <View className={totalClassNames.join(' ')}>
      <MapView style={mapStyles.map} />
    </View>
  );
};

const mapStyles = StyleSheet.create({
  map: {
    width: '100%',
    height: '100%',
  },
});
