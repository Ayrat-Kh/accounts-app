import React from 'react';
import {
  TouchableOpacity,
  type TouchableOpacityProps,
  View,
} from 'react-native';

type ButtonProps = React.PropsWithChildren<TouchableOpacityProps> & {
  leftIcon?: React.ReactNode;
  variant: 'primary' | 'secondary' | 'ghost';
  maxWidth?: boolean;
  rounded?: boolean;
};

export const Button: React.FC<ButtonProps> = ({
  children,
  leftIcon,
  variant,
  maxWidth = false,
  rounded = false,
  ...rest
}) => {
  const classNames = ['flex-1', 'flex-row', 'justify-center', 'items-center'];

  if (maxWidth) {
    classNames.push('w-full');
  }

  if (variant === 'ghost') {
    classNames.push('border-transparent py-2 px-2');
  }
  if (variant === 'primary') {
    classNames.push(
      'bg-app-primary-300 border-2 border-app-primary-700 font-bold py-2 px-2',
    );
  }
  if (variant === 'secondary') {
    classNames.push('font-bold py-2 px-4');
  }

  if (rounded) {
    classNames.push('rounded-full');
  } else {
    classNames.push('rounded-md');
  }

  return (
    <TouchableOpacity className={classNames.join(' ')} {...rest}>
      {leftIcon}

      {Boolean(leftIcon && children) && <View className="mr-2" />}

      {children}
    </TouchableOpacity>
  );
};
