import type { NavigationProp } from '@react-navigation/native';

export const AppRoutes = {
  APP_TAB: {
    name: '(app)',
    path: '/(app)',
  },
  OVERVIEW: {
    name: 'overview',
    path: '/overview',
  },
  LOGIN: {
    name: 'login',
    path: '/login',
  },
  PROFILE: {
    name: 'profile',
    path: '/profile',
  },
  ACCOUNTS_ADD: {
    name: 'accounts/add',
    path: '/accounts/add',
  },
  ACCOUNTS_UPDATE: {
    name: 'accounts/update',
    path: '/accounts/update',
  },

  ACCOUNTS_LIST: {
    name: 'accounts/index',
    path: '/accounts/index',
  },
} as const;

export type AppParamList = {
  [AppRoutes.APP_TAB.path]: undefined;
  [AppRoutes.OVERVIEW.path]: undefined;
  [AppRoutes.LOGIN.path]: undefined;
  [AppRoutes.ACCOUNTS_UPDATE.path]: { accountId: string };
  [AppRoutes.ACCOUNTS_ADD.path]: undefined;
  [AppRoutes.PROFILE.path]: undefined;
};

export type AppNavigationProp = NavigationProp<AppParamList>;
