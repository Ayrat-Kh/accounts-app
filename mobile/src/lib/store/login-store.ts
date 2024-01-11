import { deleteItemAsync, getItemAsync, setItemAsync } from 'expo-secure-store';
import { create } from 'zustand';
import { persist } from 'zustand/middleware';

type LoginState = {
  isSignedIn: boolean;
  accessToken: string;
  setAccessToken: (accessToken: string) => void;
};

export const useLoginStore = create(
  persist<LoginState>(
    (set) => ({
      isSignedIn: false,
      accessToken: '',
      setAccessToken: (accessToken: string) =>
        set({
          isSignedIn: !!accessToken,
          accessToken,
        }),
    }),
    {
      name: 'login-store',
      getStorage: () => ({
        setItem: setItemAsync,
        getItem: getItemAsync,
        removeItem: deleteItemAsync,
      }),
    },
  ),
);
