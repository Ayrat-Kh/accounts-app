import { useMutation } from '@tanstack/react-query';
import { AxiosError } from 'axios';

import { axiosInstance } from './axios';

export type ExchangeAuthCodeParams = {
  code: string;
  clientId: string;
  grantType?: string;
  codeVerifier: string;
  redirectUri: string;
  scopes: string[];
};

export type ExchangeAuthCodeResult = {
  access_token: string;
  refresh_token: string;
  expires_in: number;
  scope: string;
  token_type: string;
  id_token: string;
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
    const params = new URLSearchParams();

    params.append('scopes', scopes.join(' '));
    params.append('grant_type', grantType);
    params.append('code', code);
    params.append('client_id', clientId);
    params.append('code_verifier', codeVerifier);
    params.append('redirect_uri', redirectUri);

    const { data } = await axiosInstance.post('token', params, {
      headers: { 'content-type': 'application/x-www-form-urlencoded' },
      baseURL: 'https://oauth2.googleapis.com',
    });

    return data as ExchangeAuthCodeResult;
  } catch (error) {
    const axiosError = error as AxiosError;

    console.log('axiosError', axiosError.request);
    throw axiosError.response?.data;
  }
};

export const useExchangeGoogleAuthCode = () => {
  return useMutation({
    mutationFn: exchangeGoogleAuthCode,
  });
};
