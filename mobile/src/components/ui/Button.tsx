import type { FC, PropsWithChildren } from 'react';
import {
  TouchableOpacity,
  type TouchableOpacityProps,
  View,
} from 'react-native';

import { Text } from './Text';

type ButtonProps = PropsWithChildren<TouchableOpacityProps> & {
  leftIcon?: React.ReactNode;
  variant: 'primary' | 'secondary' | 'ghost';
  maxWidth?: boolean;
  rounded?: boolean;
};

export const Button: FC<ButtonProps> = ({
  children,
  leftIcon,
  variant,
  maxWidth = false,
  rounded = false,
  className,
  ...rest
}) => {
  const classNames: (string | undefined)[] = [
    'flex-row',
    'justify-center',
    'items-center',
  ];

  if (maxWidth) {
    classNames.push('w-full');
  }

  if (variant === 'ghost') {
    classNames.push('border-transparent py-2 px-2');
  }
  if (variant === 'primary') {
    classNames.push('bg-primary border-2 border-primary font-bold py-2 px-2');
  }
  if (variant === 'secondary') {
    classNames.push(
      'bg-secondary border-2 border-secondary font-bold py-2 px-2',
    );
  }

  if (rounded) {
    classNames.push('rounded-full');
  } else {
    classNames.push('rounded-md');
  }

  classNames.push(className);

  return (
    <TouchableOpacity {...rest} className={classNames.join(' ')}>
      {leftIcon}

      {Boolean(leftIcon && children) && <View className="mr-2" />}

      {typeof children === 'string' ? (
        <Text
          className=""
          variant="base1"
          kind="semibold"
          color={
            ['ghost', 'primary'].includes(variant) ? 'primary' : 'secondary'
          }
        >
          {children}
        </Text>
      ) : (
        children
      )}
    </TouchableOpacity>
  );
};
