import { Text } from './Text';
import { type AccessebilityLabel, isLabelAccessebilityLabel } from './utils';

type InputLabelProps = {
  label: AccessebilityLabel;
  className?: string;
};

export const InputLabel = ({ label, className }: InputLabelProps) => {
  return isLabelAccessebilityLabel(label) ? (
    label.component
  ) : (
    <Text variant="base1" color="primary-low" className={className}>
      {label}
    </Text>
  );
};
