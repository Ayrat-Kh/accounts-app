import type { NavigationProp } from '@react-navigation/native';

export const AppRoutes = {
  OVERVIEW: 'overview',
  LOGIN: 'login',
  PROFILE: 'profile',
  ACCOUNTS_ADD: 'accounts/add',
  ACCOUNTS_UPDATE: 'accounts/update',
  ACCOUNTS_LIST: 'accounts/index',
} as const;

export type AppParamList = {
  [AppRoutes.OVERVIEW]: undefined;
  [AppRoutes.LOGIN]: undefined;
  [AppRoutes.ACCOUNTS_UPDATE]: { accountId: string };
  [AppRoutes.ACCOUNTS_ADD]: undefined;
  [AppRoutes.PROFILE]: undefined;
};

export type AppNavigationProp = NavigationProp<AppParamList>;
