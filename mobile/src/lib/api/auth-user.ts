import { useMutation, useQuery } from '@tanstack/react-query';
import type { AxiosError, AxiosResponse } from 'axios';

import type {
  GoogleLoginRequest,
  UseGetMeParams,
  UserLoginResult,
  UserResult,
} from './auth-user.types';
import { axiosInstance } from './axios';

const googleAppLoginMutation = async (
  accessToken: string,
): Promise<UserLoginResult> => {
  try {
    const { data } = await axiosInstance.post<
      GoogleLoginRequest,
      AxiosResponse<UserLoginResult>
    >('/login/google-auth', {
      accessToken,
    });

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
    const { data } = await axiosInstance.get<UserResult>('/v1/me');

    return data;
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const useGetMe = (params: UseGetMeParams | undefined = undefined) =>
  useQuery({
    queryKey: ['me'],
    queryFn: getMe,
    ...params,
  });
