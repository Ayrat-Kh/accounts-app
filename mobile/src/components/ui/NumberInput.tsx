import { useState } from 'react';

import { Input, type InputProps } from './Input';
import { getInputNumberValue } from './utils';

export type NumberInputProps = Omit<InputProps, 'onChange' | 'value'> & {
  value?: number;
  onChange: (val: number) => void;
};

// This is just input representation without any controlled state
export const NumberInput = ({ value, onChange, ...rest }: NumberInputProps) => {
  const [strState, setStrState] = useState(
    isNaN(value ?? NaN) ? '' : Number(value).toString(),
  );

  const handleBlur = (): void => {
    setStrState(isNaN(value ?? NaN) ? '' : Number(value).toString());
  };

  const handleChangeText = (val: string): void => {
    const { rawString, value } = getInputNumberValue(val, strState);

    if (rawString == strState) {
      return;
    }

    setStrState(rawString);
    onChange(value);
  };

  return (
    <Input
      {...rest}
      onChange={handleChangeText}
      value={strState}
      onBlur={handleBlur}
    />
  );
};
