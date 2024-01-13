import * as SecureStore from 'expo-secure-store';
import { create } from 'zustand';
import { createJSONStorage, persist } from 'zustand/middleware';

type LoginState = {
  isSignedIn: boolean;
  accessToken: string;
  _hasHydrated: boolean;
  setAccessToken: (accessToken: string) => void;
  setHasHydrated: (hadHydrated: boolean) => void;
  logout: () => void;
};

export const useLoginStore = create(
  persist<LoginState>(
    (set) => ({
      isSignedIn: false,
      accessToken: '',
      _hasHydrated: false,
      logout: () => {
        set({
          isSignedIn: false,
          accessToken: '',
        });
      },
      setAccessToken: (accessToken: string) =>
        set({
          isSignedIn: !!accessToken,
          accessToken,
        }),
      setHasHydrated: (hasHydrated: boolean) =>
        set({
          _hasHydrated: hasHydrated,
        }),
    }),
    {
      name: 'login-store',
      onRehydrateStorage: () => {
        return (state?: LoginState) => {
          state?.setHasHydrated(true);
        };
      },
      storage: createJSONStorage(() => ({
        setItem: SecureStore.setItemAsync,
        getItem: SecureStore.getItemAsync,
        removeItem: SecureStore.deleteItemAsync,
      })),
    },
  ),
);
