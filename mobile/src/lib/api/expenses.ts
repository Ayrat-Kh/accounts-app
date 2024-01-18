import {
  type InfiniteData,
  type UndefinedInitialDataInfiniteOptions,
  useInfiniteQuery,
  useMutation,
} from '@tanstack/react-query';
import type { AxiosError } from 'axios';

import { APP_BASE_URL, axiosInstance } from './axios';
import {
  ExpensesApi,
  type ExpensesExpenseResult,
  type ExpensesExpenseUpsertDto,
  type ExpensesExpensesResult,
  type HelpersErrorResponse,
} from './open-api';

const expeneApi = new ExpensesApi(undefined, APP_BASE_URL, axiosInstance);

type GetUserExpensesParams = {
  pageParam: number;
  pageSize: number;
};

const getUserExpenses = async ({
  pageParam,
  pageSize,
}: GetUserExpensesParams): Promise<ExpensesExpensesResult> => {
  try {
    const { data } = await expeneApi.v1ExpensesGet('', pageParam, pageSize);

    return data;
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const UseGetUserExpensesKey = ['useGetUserExpenses'];

type UseGetUserExpenses = Omit<
  UndefinedInitialDataInfiniteOptions<
    ExpensesExpensesResult,
    HelpersErrorResponse,
    InfiniteData<ExpensesExpensesResult, unknown>,
    string[],
    number
  >,
  'queryFn' | 'queryKey'
>;

export const useGetUserExpenses = (options?: UseGetUserExpenses) =>
  useInfiniteQuery({
    initialPageParam: 1,
    queryKey: UseGetUserExpensesKey,
    queryFn: ({ pageParam }: { pageParam: number }) =>
      getUserExpenses({ pageParam, pageSize: 300 }),
    retry: 1,
    getNextPageParam: (lastPage) =>
      lastPage.paginationResult.currentPage <
      lastPage.paginationResult.totalPages
        ? lastPage.paginationResult.currentPage + 1
        : null,
    getPreviousPageParam: (firstPage) =>
      firstPage.paginationResult.currentPage > 1
        ? firstPage.paginationResult.currentPage - 1
        : null,
    ...options,
  });

// create
const createUserExpenses = async (
  upsert: ExpensesExpenseUpsertDto,
): Promise<ExpensesExpenseResult> => {
  try {
    const { data } = await expeneApi.v1ExpensesPost(upsert, '');
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
  expenseId: string;
  expense: ExpensesExpenseUpsertDto;
};

const updateUserExpenses = async ({
  expenseId,
  expense,
}: UpdateUserExpensesParams): Promise<ExpensesExpenseResult> => {
  try {
    const { data } = await expeneApi.v1ExpensesExpenseIdPut(
      expense,
      '',
      expenseId,
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
