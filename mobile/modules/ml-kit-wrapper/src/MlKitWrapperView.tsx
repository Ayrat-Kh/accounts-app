import { requireNativeViewManager } from 'expo-modules-core';
import * as React from 'react';

import { MlKitWrapperViewProps } from './MlKitWrapper.types';

const NativeView: React.ComponentType<MlKitWrapperViewProps> =
  requireNativeViewManager('MlKitWrapper');

export default function MlKitWrapperView(props: MlKitWrapperViewProps) {
  return <NativeView {...props} />;
}
