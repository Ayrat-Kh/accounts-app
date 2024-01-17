import {
  type UseQueryOptions,
  useMutation,
  useQuery,
} from '@tanstack/react-query';
import type { AxiosError, AxiosResponse } from 'axios';

import type { definitions } from '../api-schema';
import { axiosInstance } from './axios';

export type ExpenseResult = definitions['expenses.ExpenseResult'];
export type ExpensesResult = definitions['expenses.ExpensesResult'];
export type ExpenseDto = definitions['expenses.ExpenseDto'];
export type ExpenseItemDto = definitions['expenses.ExpenseItemDto'];
export type ErrorResponse = definitions['helpers.ErrorResponse'];

const googleAppLoginMutation = async (
  request: GoogleLoginRequest,
): Promise<definitions['auth.UserLoginResult']> => {
  try {
    const { data } = await axiosInstance.post<
      GoogleLoginRequest,
      AxiosResponse<UserLoginResult>
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

export type UserResult = definitions['user.UserResult'];

const getMe = async (): Promise<UserResult> => {
  try {
    const { data } = await axiosInstance.get<UserResult>('/v1/users/me');

    return data;
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const UseGetMeKey = ['me'];

export type UseGetMeParams = Omit<
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
