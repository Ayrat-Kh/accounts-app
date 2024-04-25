import { Text } from './Text';
import { type AccessabilityLabel, isLabelAccessabilityLabel } from './utils';

type InputLabelProps = {
  label: AccessabilityLabel;
  className?: string;
};

export const InputLabel = ({ label, className }: InputLabelProps) => {
  return isLabelAccessabilityLabel(label) ? (
    label.component
  ) : (
    <Text variant="base1" color="secondary" className={className}>
      {label}
    </Text>
  );
};
