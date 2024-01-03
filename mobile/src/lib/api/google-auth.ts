import { GOOGLE_AUTH_LOG_PREFIX } from '~/constants/google-auth';

import { ApiResponse } from './response';

export type GoogleUser = {
  email: string;
  family_name: string;
  given_name: string;
  id: string;
  locale: string;
  name: string;
  picture: string;
  verified_email: boolean;
};

export const getGoogleUserInfo = async (
  token: string,
): Promise<ApiResponse<GoogleUser>> => {
  try {
    const response = await fetch('https://www.googleapis.com/userinfo/v2/me', {
      headers: {
        Authorization: `Bearer ${token}`,
      },
    });

    return {
      success: true,
      data: await response.json(),
    };
  } catch (error) {
    console.error(`[${GOOGLE_AUTH_LOG_PREFIX}]: error fetching user`, error);
    return {
      success: false,
      error: '[${GOOGLE_AUTH_LOG_PREFIX}]: error fetching user',
    };
  }
};
