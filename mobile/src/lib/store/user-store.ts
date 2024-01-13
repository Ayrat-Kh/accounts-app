import AsyncStorage from '@react-native-async-storage/async-storage';
import { create } from 'zustand';
import { createJSONStorage, persist } from 'zustand/middleware';

import { UserDto } from '../api/auth-user.types';

export type UserState = UserDto & {
  _hasHydrated: boolean;
  setHasHydrated: (hadHydrated: boolean) => void;
  setUser: (user: UserDto) => void;
};

export const UserStoreKey = 'user-store';

export const useUserStore = create(
  persist<UserState>(
    (set) => ({
      id: '',
      email: '',
      username: '',
      googleId: '',
      _hasHydrated: false,
      setUser: (user: UserDto) => set(user),
      setHasHydrated: (hasHydrated: boolean) =>
        set({ _hasHydrated: hasHydrated }),
    }),
    {
      name: 'user-store',
      onRehydrateStorage: () => (state?: UserState) => {
        state?.setHasHydrated(true);
      },
      storage: createJSONStorage(() => AsyncStorage),
    },
  ),
);
