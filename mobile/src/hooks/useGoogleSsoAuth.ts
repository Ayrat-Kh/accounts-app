import * as Google from 'expo-auth-session/providers/google';

import { GOOGLE_AUTH_LOG_PREFIX } from '~/constants/google-auth';
import { useGoogleAppLoginMutation } from '~/lib/api/auth-user';
import {
  type ExchangeAuthCodeResult,
  useExchangeGoogleAuthCode,
} from '~/lib/api/google';
import type { AuthUserLoginResult } from '~/lib/api/open-api';
import { useLoginStore } from '~/lib/store';

const GOOGLE_CLIENT_ID = process.env.EXPO_PUBLIC_GOOGLE_CLIENT_ID;

type LoginResult =
  | {
      result: Exclude<AuthStatus, 'SUCCESS'>;
    }
  | {
      result: 'SUCCESS';
      userInfo: AuthUserLoginResult;
    };

type UseAuthResult = () => Promise<LoginResult>;

type AuthStatus =
  | 'SUCCESS'
  | 'CANCELLED'
  | 'ERROR_EMPTY_TOKEN'
  | 'ERROR_GOOGLE'
  | 'ERROR_USER_FETCH';

export const useGoogleSsoAuth = (): UseAuthResult => {
  const { mutateAsync: exchangeToken } = useExchangeGoogleAuthCode();

  const [authorizationRequest /*response */, , prompt] =
    Google.useIdTokenAuthRequest({
      iosClientId: GOOGLE_CLIENT_ID,
      scopes: [
        'https://www.googleapis.com/auth/userinfo.profile',
        'https://www.googleapis.com/auth/userinfo.email',
      ],
      shouldAutoExchangeCode: false,
    });

  const { mutateAsync: googleAppLogin } = useGoogleAppLoginMutation();

  return async (): Promise<LoginResult> => {
    let exchangeTokenResult: ExchangeAuthCodeResult;

    try {
      const authorizationResult = await prompt({});

      if (authorizationResult.type !== 'success') {
        return {
          result: 'CANCELLED',
        };
      }

      const exchangeTokenRequest = {
        scopes: authorizationRequest?.scopes ?? [],
        code: authorizationResult.params.code,
        clientId: authorizationRequest?.clientId ?? '',
        codeVerifier: authorizationRequest?.codeVerifier ?? '',
        redirectUri: authorizationRequest?.redirectUri ?? '',
      };

      exchangeTokenResult = await exchangeToken(exchangeTokenRequest);

      console.log('id token = ', exchangeTokenResult.idToken);
    } catch (error) {
      console.error(`[${GOOGLE_AUTH_LOG_PREFIX}] error google login`, error);
      return {
        result: 'ERROR_GOOGLE',
      };
    }

    try {
      const userInfo = await googleAppLogin({
        idToken: exchangeTokenResult.idToken,
      });

      useLoginStore.getState().setAccessToken(userInfo.accessToken);

      return {
        result: 'SUCCESS',
        userInfo: userInfo,
      };
    } catch (e) {
      console.error(`[${GOOGLE_AUTH_LOG_PREFIX}] error app login`, e);
      return {
        result: 'ERROR_USER_FETCH',
      };
    }
  };
};
