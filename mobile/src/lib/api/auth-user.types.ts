import type { UseQueryOptions } from '@tanstack/react-query';

import type { ErrorResponse } from './response.types';

export type UserDto = {
  id: string;
  username: string;
  email: string;
  googleId: string;
};

export type GoogleLoginRequest = {
  accessToken: string;
};

export type UserResult = {
  user: UserDto;
};

export type UserLoginResult = UserResult & { accessToken: string };

export type UseGetMeParams = Omit<
  UseQueryOptions<UserResult, ErrorResponse>,
  'queryFn' | 'queryKey'
>;
