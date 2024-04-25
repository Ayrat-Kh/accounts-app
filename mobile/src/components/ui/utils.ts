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

export type InputSizeVariant = 'lg' | 'md' | 'sm' | 'any';

export const inputVariantSize: Record<InputSizeVariant, string> = {
  lg: 'h-[60]',
  md: 'h-[48]',
  sm: 'h-[32]',
  any: '',
};
