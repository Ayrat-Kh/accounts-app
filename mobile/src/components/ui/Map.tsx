import { type FC, type LegacyRef } from 'react';
import { StyleSheet, View } from 'react-native';
import MapView, { type MapViewProps } from 'react-native-maps';

type MapProps = MapViewProps & {
  className?: string;
  mapRef?: LegacyRef<MapView> | undefined;
};

export const Map: FC<MapProps> = ({ className, mapRef }) => {
  const totalClassNames: (string | unknown)[] = ['rounded', className];

  return (
    <View className={totalClassNames.join(' ')}>
      <MapView ref={mapRef} style={mapStyles.map} />
    </View>
  );
};

const mapStyles = StyleSheet.create({
  map: {
    width: '100%',
    height: '100%',
  },
});
