import * as Location from 'expo-location';
import { useEffect, useRef, useState } from 'react';
import { useFormContext, useWatch } from 'react-hook-form';
import { View } from 'react-native';
import MapView from 'react-native-maps';

import { IconScrollV } from '~/assets/icons';
import { Button, InputLabel, Map } from '~/components/ui';
import type { ExpensesExpenseDto } from '~/lib/api/open-api';

export const MapInput = () => {
  const [scrollEnabled, setScrollEnabled] = useState(false);

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
      <View className="relative ">
        <Map
          scrollEnabled={scrollEnabled}
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

        <Button variant="ghost" className="absolute left-2 top-2">
          {/* <Canvas style={{ flex: 1 }}>
            <Group blendMode="multiply">
              <Circle cx={r} cy={r} r={r} color="cyan" />
              <Circle cx={c} cy={r} r={r} color="magenta" />
              <Circle cx={size / 2} cy={c} r={r} color="yellow" />
            </Group>
          </Canvas> */}
          <IconScrollV className="left-0 top-0" color="secondary" size={32} />
        </Button>
      </View>
    </View>
  );
};
