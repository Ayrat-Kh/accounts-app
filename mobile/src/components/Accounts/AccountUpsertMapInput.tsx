import * as Location from 'expo-location';
import { useEffect, useRef, useState } from 'react';
import { useFormContext } from 'react-hook-form';
import { View } from 'react-native';
import MapView, { type Region } from 'react-native-maps';

import { IconLocation, IconScrollV } from '~/assets/icons';
import { Button, InputLabel, Map } from '~/components/ui';
import type { UpsertAccount } from '~/lib/api/open-api';

export const AccountUpsertMapInput = () => {
  const [scrollEnabled, setScrollEnabled] = useState(false);

  const mapRef = useRef<MapView>(null);
  const { setValue } = useFormContext<UpsertAccount>();

  const handleRegionChange = async (region: Region) => {
    setValue('address', {
      lat: region.latitude,
      lng: region.longitude,
      address: '',
    });
  };

  useEffect(() => {
    async function init() {
      const { status } = await Location.requestForegroundPermissionsAsync();

      if (status !== 'granted') {
        return;
      }

      const location = await Location.getCurrentPositionAsync({});

      const { coords } = location;

      mapRef.current?.animateToRegion(
        {
          latitude: coords.latitude,
          longitude: coords.longitude,
          // zoom
          latitudeDelta: 0.001,
          longitudeDelta: 0.001,
        },
        3000,
      );

      const address = await mapRef.current?.addressForCoordinate(coords);

      setValue('name', address?.name ?? '');
      setValue('address', {
        // duration
        lat: coords.latitude,
        lng: coords.longitude,
        address: '',
      });
    }
    init();
  }, []);

  return (
    <View>
      <InputLabel label="Paid location" className="mb-1 mt-2" />
      <View className="relative items-center justify-center top-0 left-0 right-0 bottom-0">
        <Map
          scrollEnabled={scrollEnabled}
          mapRef={mapRef}
          className="h-[300] w-full"
          onRegionChange={handleRegionChange}
        />

        <View className="absolute">
          <IconLocation color="secondary" size={40} />
        </View>

        <Button
          variant="secondary"
          size="any"
          className="absolute top-2 left-2"
          icon={<IconScrollV isCrossed={scrollEnabled} size={26} />}
          onPress={() => setScrollEnabled((prev) => !prev)}
        />
      </View>
    </View>
  );
};
