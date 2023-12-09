import { NativeModulesProxy, EventEmitter, Subscription } from 'expo-modules-core';

// Import the native module. On web, it will be resolved to MlKitWrapper.web.ts
// and on native platforms to MlKitWrapper.ts
import MlKitWrapperModule from './src/MlKitWrapperModule';
import MlKitWrapperView from './src/MlKitWrapperView';
import { ChangeEventPayload, MlKitWrapperViewProps } from './src/MlKitWrapper.types';

// Get the native constant value.
export const PI = MlKitWrapperModule.PI;

export function hello(): string {
  return MlKitWrapperModule.hello();
}

export async function setValueAsync(value: string) {
  return await MlKitWrapperModule.setValueAsync(value);
}

const emitter = new EventEmitter(MlKitWrapperModule ?? NativeModulesProxy.MlKitWrapper);

export function addChangeListener(listener: (event: ChangeEventPayload) => void): Subscription {
  return emitter.addListener<ChangeEventPayload>('onChange', listener);
}

export { MlKitWrapperView, MlKitWrapperViewProps, ChangeEventPayload };
