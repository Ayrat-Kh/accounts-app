import React from 'react';
import { type TouchableOpacityProps, View } from 'react-native';

import { ButtonContainer } from './Button.styles';

type ButtonProps = React.PropsWithChildren<TouchableOpacityProps> & {
  leftIcon?: React.ReactNode;
  variant: 'primary' | 'secondary' | 'ghost';
};

export const Button: React.FC<ButtonProps> = ({
  children,
  leftIcon,
  variant,
  ...rest
}) => {
  const classNames = [];

  if (variant === 'ghost') {
    classNames.push('bg-transparent border-transparent');
  }
  if (variant === 'primary') {
    classNames.push('font-bold');
  }
  if (variant === 'secondary') {
    classNames.push('font-bold');
  }

  return (
    <ButtonContainer {...rest} className={classNames.join(' ')}>
      {leftIcon}

      {Boolean(leftIcon && children) && <View className="mr-2" />}

      {children}
    </ButtonContainer>
  );
};
