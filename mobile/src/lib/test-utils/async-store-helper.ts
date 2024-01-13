import AsyncStorage from '@react-native-async-storage/async-storage';

const getItem = AsyncStorage.getItem as jest.Mock;
const setItem = AsyncStorage.setItem as jest.Mock;
const removeItem = AsyncStorage.removeItem as jest.Mock;

export const AsyncStorageMock = {
  getItem,
  setItem,
  removeItem,
};
