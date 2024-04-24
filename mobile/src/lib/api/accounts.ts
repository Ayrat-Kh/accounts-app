import {
  type InfiniteData,
  type UndefinedInitialDataInfiniteOptions,
  useInfiniteQuery,
  useMutation,
} from '@tanstack/react-query';
import type { AxiosError } from 'axios';

import { APP_BASE_URL, axiosInstance } from './axios';
import {
  type AccountResult,
  AccountsApi,
  type ErrorResponse,
  type UpsertAccount,
  type UserAccountsResult,
} from './open-api';

const accountsApi = new AccountsApi(undefined, APP_BASE_URL, axiosInstance);

type GetUserExpensesParams = {
  pageParam: number;
  pageSize: number;
  userId: string;
};

const getUserExpenses = async ({
  pageParam,
  pageSize,
  userId,
}: GetUserExpensesParams): Promise<UserAccountsResult> => {
  try {
    const { data } = await accountsApi.v1UsersUserIdAccountsGet(
      '',
      pageParam,
      pageSize,
      userId,
    );

    return data;
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const UseGetUserExpensesKey = ['useGetUserExpenses'];

type UseGetUserExpenses = Omit<
  UndefinedInitialDataInfiniteOptions<
    UserAccountsResult,
    ErrorResponse,
    InfiniteData<UserAccountsResult, unknown>,
    string[],
    number
  >,
  'queryFn' | 'queryKey'
>;

export const useGetUserExpenses = (
  userId: string,
  options?: UseGetUserExpenses,
) =>
  useInfiniteQuery({
    initialPageParam: 1,
    queryKey: UseGetUserExpensesKey,
    queryFn: ({ pageParam }: { pageParam: number }) =>
      getUserExpenses({ pageParam, pageSize: 300, userId }),
    retry: 1,
    getNextPageParam: (lastPage) =>
      // lastPage. .paginationResult.currentPage <
      // lastPage.paginationResult.totalPages
      //   ? lastPage.paginationResult.currentPage + 1
      //   : null,
      null,
    getPreviousPageParam: (firstPage) =>
      // firstPage.paginationResult.currentPage > 1
      //   ? firstPage.paginationResult.currentPage - 1
      //   : null,
      null,
    ...options,
  });

// create
const createUserExpenses = async (
  upsert: UpsertAccount,
): Promise<AccountResult> => {
  try {
    const { data } = await accountsApi.v1AccountsPost(upsert, '');
    return data;
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const useCreateUserExpenses = () =>
  useMutation({
    mutationFn: createUserExpenses,
  });

type UpdateUserExpensesParams = {
  accountId: string;
  account: UpsertAccount;
};

const updateUserExpenses = async ({
  accountId,
  account,
}: UpdateUserExpensesParams): Promise<AccountResult> => {
  try {
    const { data } = await accountsApi.v1AccountsAccountIdPut(
      account,
      '',
      accountId,
    );
    return data;
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const useUpdateUserExpenses = () =>
  useMutation({
    mutationFn: updateUserExpenses,
  });
