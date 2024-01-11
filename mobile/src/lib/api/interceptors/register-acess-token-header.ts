import type { Axios } from 'axios';

import { useLoginStore } from '~/lib/store';

export const registerAccessTokenHeader = (axios: Axios) => {
  axios.interceptors.request.use(function (config) {
    const accessToken = useLoginStore.getState().accessToken;

    if (accessToken) {
      config.headers.Authorization = `Bearer ${accessToken}`;
    }

    return config;
  });
};
