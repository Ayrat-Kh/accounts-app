import {
  type UseQueryOptions,
  useMutation,
  useQuery,
} from '@tanstack/react-query';
import type { AxiosError, AxiosResponse } from 'axios';

import { axiosInstance } from './axios';
import type { AuthGoogleLoginRequest } from './models/auth-google-login-request';
import type { AuthUserLoginResult } from './models/auth-user-login-result';
import type { HelpersErrorResponse } from './models/helpers-error-response';
import type { UserUserResult } from './models/user-user-result';

const googleAppLoginMutation = async (
  request: AuthGoogleLoginRequest,
): Promise<AuthUserLoginResult> => {
  try {
    const { data } = await axiosInstance.post<
      AuthGoogleLoginRequest,
      AxiosResponse<AuthUserLoginResult>
    >('/login/google-auth', request);

    return data;
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const useGoogleAppLoginMutation = () =>
  useMutation({
    mutationFn: googleAppLoginMutation,
  });

const getMe = async (): Promise<UserUserResult> => {
  try {
    const { data } = await axiosInstance.get<UserUserResult>('/v1/users/me');

    return data;
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const UseGetMeKey = ['me'];

export type UseGetMeParams = Omit<
  UseQueryOptions<UserUserResult, HelpersErrorResponse>,
  'queryFn' | 'queryKey'
>;

export const useGetMe = (params: UseGetMeParams | undefined = undefined) =>
  useQuery({
    queryKey: UseGetMeKey,
    queryFn: getMe,
    retry: 1,
    ...params,
  });
