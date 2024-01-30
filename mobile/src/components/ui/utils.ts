type AccessabilityLabelObject = {
  component: React.ReactNode;
  accessabilityLabel: string;
};

export type AccessabilityLabel = AccessabilityLabelObject | string;

export const isLabelAccessabilityLabel = (
  obj: AccessabilityLabel,
): obj is AccessabilityLabelObject => {
  return typeof obj === 'object';
};

export const getAccessabilityLabelNode = (
  obj: AccessabilityLabel,
): React.ReactNode => {
  return isLabelAccessabilityLabel(obj) ? obj.component : obj;
};
