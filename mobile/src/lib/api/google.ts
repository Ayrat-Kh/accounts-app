import { useMutation } from '@tanstack/react-query';
import { AxiosError, AxiosResponse } from 'axios';

import { axiosInstance } from './axios';

export type ExchangeAuthCodeParams = {
  code: string;
  clientId: string;
  grantType?: string;
  codeVerifier: string;
  redirectUri: string;
  scopes: string[];
};

type ExchangeAuthCodeRawResult = {
  access_token: string;
  refresh_token: string;
  expires_in: number;
  scope: string;
  token_type: string;
  id_token: string;
};

export type ExchangeAuthCodeResult = {
  accessToken: string;
  refreshToken: string;
  expiresIn: number;
  scope: string;
  tokenType: string;
  idToken: string;
};

const exchangeGoogleAuthCode = async ({
  clientId,
  codeVerifier,
  redirectUri,
  grantType = 'authorization_code',
  scopes,
  code,
}: ExchangeAuthCodeParams): Promise<ExchangeAuthCodeResult> => {
  try {
    const bodyParams = {
      scopes: scopes.join(' '),
      grant_type: grantType,
      code,
      client_id: clientId,
      code_verifier: codeVerifier,
      redirect_uri: redirectUri,
    };

    const { data } = await axiosInstance.post<
      typeof bodyParams,
      AxiosResponse<ExchangeAuthCodeRawResult>
    >('token', bodyParams, {
      baseURL: 'https://oauth2.googleapis.com',
    });

    return {
      accessToken: data.access_token,
      refreshToken: data.refresh_token,
      expiresIn: data.expires_in,
      scope: data.scope,
      tokenType: data.token_type,
      idToken: data.id_token,
    };
  } catch (error) {
    const axiosError = error as AxiosError;
    throw axiosError.response?.data;
  }
};

export const useExchangeGoogleAuthCode = () => {
  return useMutation({
    mutationFn: exchangeGoogleAuthCode,
  });
};
