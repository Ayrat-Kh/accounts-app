import axios from 'axios';

export const APP_BASE_URL = process.env.EXPO_PUBLIC_APP_BASE_URL;

export const axiosInstance = axios.create({
  baseURL: APP_BASE_URL,
});
