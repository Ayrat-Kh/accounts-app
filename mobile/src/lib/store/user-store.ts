import AsyncStorage from '@react-native-async-storage/async-storage';
import { create } from 'zustand';
import { persist } from 'zustand/middleware';

import { UserDto } from '../api/auth-user.types';

export type UserState = UserDto & {
  setUser: (user: UserDto) => void;
};

export const useUserStore = create(
  persist<UserState>(
    (set) => ({
      id: '',
      email: '',
      username: '',
      googleId: '',
      setUser: (user: UserDto) => set(user),
    }),
    {
      name: 'user-store',
      getStorage: () => AsyncStorage,
    },
  ),
);
