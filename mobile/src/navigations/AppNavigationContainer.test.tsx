import { QueryClientProvider } from '@tanstack/react-query';
import { render, waitFor } from '@testing-library/react-native';
import React from 'react';

import { UseGetMeKey } from '~/lib/api/auth-user';
import { queryClient } from '~/lib/api/queryClient';

import { AppNavigationContainer } from './AppNavigationContainer';
import * as HomeScreenModule from './BottomTabNavigator';
import * as LoginNavigatorModule from './LoginNavigator';
import {
  getStoreGetItemMock,
  getStoreRemoveItemMock,
  getStoreSetItemMock,
} from './helpers/async-store-helper';
import { loggedOutUserMock } from './helpers/login-store-mock';
import {
  getSecureStoreGetItemMock,
  getSecureStoreRemoveItemMock,
  getSecureStoreSetItemMock,
} from './helpers/secure-store-helper';
import { userMock } from './helpers/user-store-mock';

getStoreGetItemMock();
getStoreRemoveItemMock();
getStoreSetItemMock();

getSecureStoreGetItemMock();
getSecureStoreRemoveItemMock();
getSecureStoreSetItemMock();

// const HomeScreenComponent = jest
//   .spyOn(HomeScreenModule, 'BottomTabNavigator')
//   .mockImplementation(() => <></>);

// const LoginNavigatorComponent = jest
//   .spyOn(LoginNavigatorModule, 'LoginNavigator')
//   .mockImplementation(() => <></>);

describe('AppNavigationContainer', () => {
  beforeEach(() => {
    queryClient.setQueryData(UseGetMeKey, {
      user: userMock,
    });
    jest.clearAllMocks();
  });

  it('Should navigate to home screen', async () => {
    // render(
    //   <QueryClientProvider client={queryClient}>
    //     <AppNavigationContainer />
    //   </QueryClientProvider>,
    // );
    // await waitFor(() => expect(HomeScreenComponent).toHaveBeenCalled());
  });

  it('Should navigate to login screen', async () => {
    // getSecureStoreGetItemMock().mockImplementation(() =>
    //   Promise.resolve(
    //     JSON.stringify({
    //       state: loggedOutUserMock,
    //     }),
    //   ),
    // );
    // render(
    //   <QueryClientProvider client={queryClient}>
    //     <AppNavigationContainer />
    //   </QueryClientProvider>,
    // );
    // await waitFor(() => expect(LoginNavigatorComponent).toHaveBeenCalled());
  });
});
