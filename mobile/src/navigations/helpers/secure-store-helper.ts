import * as SecureStore from 'expo-secure-store';

import { loggedInUserMock } from './login-store-mock';

export const getSecureStoreSetItemMock = () =>
  jest.spyOn(SecureStore, 'setItemAsync');

export const getSecureStoreGetItemMock = () =>
  jest.spyOn(SecureStore, 'getItemAsync').mockImplementation(() => {
    return Promise.resolve(
      JSON.stringify({
        state: loggedInUserMock,
        version: 0,
      }),
    );
  });

export const getSecureStoreRemoveItemMock = () =>
  jest.spyOn(SecureStore, 'deleteItemAsync');
