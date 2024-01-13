import { QueryClientProvider } from '@tanstack/react-query';
import { render, waitFor } from '@testing-library/react-native';
import React from 'react';

import {
  AsyncStorageMock,
  mockLoginStore, // SecureStorageMock,
  userDataMocks,
} from '~/lib/test-utils';
import { loggedOutUserMock } from '~/lib/test-utils/login-store-mock';

import { UseGetMeKey } from '~/lib/api/auth-user';
import { queryClient } from '~/lib/api/queryClient';
import { useLoginStore } from '~/lib/store';

import { AppNavigationContainer } from './AppNavigationContainer';
import * as HomeScreenModule from './BottomTabNavigator';
import * as LoginNavigatorModule from './LoginNavigator';

AsyncStorageMock.getItem.mockImplementation(() =>
  Promise.resolve(
    JSON.stringify({
      state: {},
    }),
  ),
);

const HomeScreenComponent = jest
  .spyOn(HomeScreenModule, 'BottomTabNavigator')
  .mockImplementation(() => <></>);

const LoginNavigatorComponent = jest
  .spyOn(LoginNavigatorModule, 'LoginNavigator')
  .mockImplementation(() => <></>);

describe('AppNavigationContainer', () => {
  beforeEach(async () => {
    queryClient.setQueryData(UseGetMeKey, {
      user: userDataMocks.userMock,
    });

    await useLoginStore.persist.rehydrate();
    jest.clearAllMocks();
    await mockLoginStore();
  });

  it('Should navigate to home screen', async () => {
    render(
      <QueryClientProvider client={queryClient}>
        <AppNavigationContainer />
      </QueryClientProvider>,
    );
    await waitFor(() => expect(HomeScreenComponent).toHaveBeenCalled());
  });

  it('Should navigate to login screen', async () => {
    await mockLoginStore(loggedOutUserMock);
    render(
      <QueryClientProvider client={queryClient}>
        <AppNavigationContainer />
      </QueryClientProvider>,
    );
    await waitFor(() => expect(LoginNavigatorComponent).toHaveBeenCalled());
  });
});
