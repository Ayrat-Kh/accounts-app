import {
  type UseQueryOptions,
  useMutation,
  useQuery,
} from '@tanstack/react-query';
import type { AxiosError } from 'axios';

import { APP_BASE_URL, axiosInstance } from './axios';
import {
  AuthApi,
  type ErrorResponse,
  type GoogleLoginRequest,
  type UserLoginResult,
  type UserResult,
  UsersApi,
} from './open-api';

const authApi = new AuthApi(undefined, APP_BASE_URL, axiosInstance);
const userApi = new UsersApi(undefined, APP_BASE_URL, axiosInstance);

const googleAppLoginMutation = async (
  request: GoogleLoginRequest,
): Promise<UserLoginResult> => {
  try {
    const { data } = await authApi.loginGoogleAuthCallbackPost(request);
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

const getMe = async (): Promise<UserResult> => {
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
  UseQueryOptions<UserResult, ErrorResponse>,
  'queryFn' | 'queryKey'
>;

export const useGetMe = (params: UseGetMeParams | undefined = undefined) =>
  useQuery({
    queryKey: UseGetMeKey,
    queryFn: getMe,
    retry: 1,
    ...params,
  });
