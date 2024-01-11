/// <reference types="nativewind/types" />

declare global {
  namespace NodeJS {
    interface ProcessEnv {
      GOOGLE_CLIENT_ID: string;
      APP_BASE_URL: string;
    }
  }
}
