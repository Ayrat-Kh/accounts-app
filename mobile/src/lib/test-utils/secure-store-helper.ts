import { deleteItemAsync, getItemAsync, setItemAsync } from 'expo-secure-store';

import { useLoginStore } from '../store';
import { loggedInUserMock } from './login-store-mock';

const getItem = getItemAsync as jest.Mock;
const setItem = setItemAsync as jest.Mock;
const removeItem = deleteItemAsync as jest.Mock;

export const SecureStorageMock = {
  getItem,
  setItem,
  removeItem,
};

export const mockLoginStore = async (data = loggedInUserMock) => {
  getItem.mockResolvedValue(
    JSON.stringify({
      state: data,
      version: 0,
    }),
  );
  await useLoginStore.persist.rehydrate();
};
