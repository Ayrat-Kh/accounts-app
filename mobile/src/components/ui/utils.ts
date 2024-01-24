type AccessebilityLabelObject = {
  component: React.ReactNode;
  accessebilityLabel: string;
};

export type AccessebilityLabel = AccessebilityLabelObject | string;

export const isLabelAccessebilityLabel = (
  obj: AccessebilityLabel,
): obj is AccessebilityLabelObject => {
  return typeof obj === 'object';
};

export const getAccessebilityLabelNode = (
  obj: AccessebilityLabel,
): React.ReactNode => {
  return isLabelAccessebilityLabel(obj) ? obj.component : obj;
};
