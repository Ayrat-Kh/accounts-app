import * as Google from 'expo-auth-session/providers/google';
import { useCallback } from 'react';

import { GOOGLE_AUTH_LOG_PREFIX } from '~/constants/google-auth';
import { useGoogleAppLoginMutation } from '~/lib/api/auth-user';
import { UserLoginResult } from '~/lib/api/auth-user.types';

const GOOGLE_CLIENT_ID =
  process.env.GOOGLE_CLIENT_ID ??
  '402089960208-33i57vpi00o42cckhga0lc222lopbvk1.apps.googleusercontent.com'; // todo: remove me

type LoginResult =
  | {
      result: Exclude<AuthStatus, 'SUCCESS'>;
    }
  | {
      result: 'SUCCESS';
      userInfo: UserLoginResult;
    };

type UseAuthResult = () => Promise<LoginResult>;

type AuthStatus =
  | 'SUCCESS'
  | 'CANCELLED'
  | 'ERROR_EMPTY_TOKEN'
  | 'ERROR_GOOGLE'
  | 'ERROR_USER_FETCH';

export const useGoogleSsoAuth = (): UseAuthResult => {
  const [, /** request */ response, prompt] = Google.useAuthRequest({
    iosClientId: GOOGLE_CLIENT_ID,
    scopes: [
      'https://www.googleapis.com/auth/userinfo.profile',
      'https://www.googleapis.com/auth/userinfo.email',
    ],
  });

  const { mutateAsync: googleAppLogin } = useGoogleAppLoginMutation();

  return useCallback(async (): Promise<LoginResult> => {
    try {
      await prompt();
    } catch (error) {
      console.error(`[${GOOGLE_AUTH_LOG_PREFIX}] error login #1`, error);
      return {
        result: 'ERROR_GOOGLE',
      };
    }

    if (response?.type === 'cancel' || response?.type === 'dismiss') {
      console.error(`[${GOOGLE_AUTH_LOG_PREFIX}] cancelled`);
      return {
        result: 'CANCELLED',
      };
    }

    console.log('response?.type', response?.type);

    if (response?.type !== 'success') {
      console.error(`[${GOOGLE_AUTH_LOG_PREFIX}] error login #2`);
      return {
        result: 'ERROR_GOOGLE',
      };
    }

    if (!response.authentication) {
      console.error(`[${GOOGLE_AUTH_LOG_PREFIX}] error empty token`);
      return {
        result: 'ERROR_EMPTY_TOKEN',
      };
    }

    console.log(
      'response.authentication.accessToken',
      response.authentication.accessToken,
    );

    try {
      const userInfo = await googleAppLogin(
        response.authentication.accessToken,
      );
      return {
        result: 'SUCCESS',
        userInfo: userInfo,
      };
    } catch (e) {
      return {
        result: 'ERROR_USER_FETCH',
      };
    }
  }, [response, prompt]);
};
