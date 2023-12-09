import * as React from 'react';

import { MlKitWrapperViewProps } from './MlKitWrapper.types';

export default function MlKitWrapperView(props: MlKitWrapperViewProps) {
  return (
    <div>
      <span>{props.name}</span>
    </div>
  );
}
