// Add this to import TypeScript files
import { ExpoConfig } from 'expo/config';
import 'ts-node/register';

// In SDK 46 and lower, use the following import instead:
// import { ExpoConfig } from '@expo/config-types';

const config: ExpoConfig = {
  name: 'Accounts app',
  slug: 'accounts-app',
  scheme: 'accounts-app',
  version: '1.0.0',
  orientation: 'portrait',
  icon: './assets/icon.png',
  userInterfaceStyle: 'light',
  splash: {
    image: './assets/splash.png',
    resizeMode: 'contain',
    backgroundColor: '#ffffff',
  },
  assetBundlePatterns: ['**/*'],
  extra: {
    eas: {
      projectId: '0991edb4-dba6-4e07-a778-748967fb7bf5',
    },
  },
  android: {
    adaptiveIcon: {
      foregroundImage: './assets/adaptive-icon.png',
      backgroundColor: '#ffffff',
    },
    package: 'com.airatkhisamiev.accountsapp',
  },
  ios: {
    bundleIdentifier: 'com.airatkhisamiev.accountsapp',
  },
  plugins: [
    [
      'expo-build-properties',
      {
        android: {
          compileSdkVersion: 31,
          targetSdkVersion: 31,
          buildToolsVersion: '31.0.0',
        },
        ios: {
          deploymentTarget: '14.0',
        },
      },
    ],
    [
      'expo-location',
      {
        locationWhenInUsePermission:
          'Allow $(PRODUCT_NAME) to use your location.',
      },
    ],
    'expo-router',
  ],
};

export default config;
