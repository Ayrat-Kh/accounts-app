import { QueryClientProvider } from '@tanstack/react-query';
import { render, waitFor } from '@testing-library/react-native';
import * as expoRouter from 'expo-router';

import {
  AsyncStorageMock,
  mockLoginStore,
  userDataMocks,
} from '~/lib/test-utils';
import { loggedOutUserMock } from '~/lib/test-utils/login-store-mock';

import { AppRoutes } from '~/constants/routes';
import { UseGetMeKey } from '~/lib/api/authUser';
import { queryClient } from '~/lib/api/queryClient';
import { useLoginStore } from '~/lib/store';

import { AuthProviderInner } from './AuthProviderInner';

AsyncStorageMock.getItem.mockImplementation(() =>
  Promise.resolve(
    JSON.stringify({
      state: {},
    }),
  ),
);

const RedirectComponent = jest
  .spyOn(expoRouter, 'Redirect')
  .mockImplementation(() => null);

describe('AuthProviderInner', () => {
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
        <AuthProviderInner />
      </QueryClientProvider>,
    );
    await waitFor(() =>
      expect(RedirectComponent).toHaveBeenCalledWith(
        { href: AppRoutes.OVERVIEW },
        {},
      ),
    );
  });

  it('Should navigate to login screen', async () => {
    await mockLoginStore(loggedOutUserMock);
    render(
      <QueryClientProvider client={queryClient}>
        <AuthProviderInner />
      </QueryClientProvider>,
    );
    await waitFor(() =>
      expect(RedirectComponent).toHaveBeenCalledWith(
        { href: AppRoutes.LOGIN },
        {},
      ),
    );
  });
});
