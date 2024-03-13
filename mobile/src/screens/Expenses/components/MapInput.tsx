import * as Location from 'expo-location';
import { useEffect, useRef } from 'react';
import { useFormContext, useWatch } from 'react-hook-form';
import { View } from 'react-native';
import MapView from 'react-native-maps';

import { InputLabel, Map } from '~/components/ui';
import type { ExpensesExpenseDto } from '~/lib/api/open-api';

export const MapInput = () => {
  const mapRef = useRef<MapView>(null);
  const { control, setValue } = useFormContext<ExpensesExpenseDto>();
  const coord = useWatch({
    control,
    name: 'coord',
  });

  useEffect(() => {
    async function init() {
      const { status } = await Location.requestForegroundPermissionsAsync();
      if (status !== 'granted') {
        return;
      }

      const location = await Location.getCurrentPositionAsync({});

      const { coords } = location;

      console.log('coords', coords);

      mapRef.current?.animateToRegion(
        {
          latitude: coords.latitude,
          longitude: coords.longitude,
          latitudeDelta: 0.01,
          longitudeDelta: 0.01,
        },
        3000,
      );

      setValue('coord', {
        lat: coords.latitude,
        lng: coords.longitude,
      });
    }
    init();
  }, []);

  return (
    <View>
      <InputLabel label="Paid location" className="mb-1 mt-2" />
      <Map
        mapRef={mapRef}
        className="h-[300]"
        camera={{
          zoom: 13,
          center: {
            latitude: coord?.lat ?? 0,
            longitude: coord?.lng ?? 0,
          },
          pitch: 0,
          heading: 0,
        }}
      />
    </View>
  );
};
