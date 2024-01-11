import axios from 'axios';

import { registerAccessTokenHeader } from './interceptors/register-acess-token-header';

const APP_BASE_URL = process.env.APP_BASE_URL ?? 'http://localhost:3000';

export const axiosInstance = axios.create({
  baseURL: APP_BASE_URL,
});

registerAccessTokenHeader(axiosInstance);
