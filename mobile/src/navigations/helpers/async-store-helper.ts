import AsyncStorage from '@react-native-async-storage/async-storage';

import { UserStoreKey } from '~/lib/store';

import { userMock } from './user-store-mock';

jest.mock('@react-native-async-storage/async-storage', () => ({
  getItem: jest.fn(),
  setItem: jest.fn(),
  removeItem: jest.fn(),
}));

export const getStoreSetItemMock = () => jest.spyOn(AsyncStorage, 'setItem');

export const getStoreGetItemMock = () =>
  jest.spyOn(AsyncStorage, 'getItem').mockImplementation((key: string) => {
    if (key === UserStoreKey) {
      return Promise.resolve(
        JSON.stringify({
          state: userMock,
        }),
      );
    }

    return Promise.resolve(
      JSON.stringify({
        state: {},
      }),
    );
  });

export const getStoreRemoveItemMock = () =>
  jest.spyOn(AsyncStorage, 'removeItem');
