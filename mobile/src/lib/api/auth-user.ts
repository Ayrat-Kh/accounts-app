import {
  type UseQueryOptions,
  useMutation,
  useQuery,
} from '@tanstack/react-query';
import type { AxiosError } from 'axios';

import { axiosInstance } from './axios';
import {
  AuthApi,
  type AuthGoogleLoginRequest,
  type AuthUserLoginResult,
  type HelpersErrorResponse,
  UserApi,
  type UserUserResult,
} from './open-api';

const authApi = new AuthApi(undefined, undefined, axiosInstance);
const userApi = new UserApi(undefined, undefined, axiosInstance);

const googleAppLoginMutation = async (
  request: AuthGoogleLoginRequest,
): Promise<AuthUserLoginResult> => {
  try {
    const { data } = await authApi.loginGoogleAuthPost(request);
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
    // authorization should be empty token will be passed in interceptor
    const { data } = await userApi.v1UsersUserIdGet('', 'me');

    return data;
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const UseGetMeKey = ['me'];

type UseGetMeParams = Omit<
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
