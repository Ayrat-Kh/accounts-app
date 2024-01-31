import type { FC, PropsWithChildren } from 'react';
import {
  TouchableOpacity,
  type TouchableOpacityProps,
  View,
} from 'react-native';

import { Text, TextColor, type TextKind } from './Text';

type ButtonVariant = 'primary' | 'secondary' | 'ghost' | 'input';
const buttonVariants: Record<ButtonVariant, string> = {
  primary: 'bg-compPrimary border-2 border-primary font-bold py-2 px-2',
  secondary: 'bg-compSecondary border-2 border-secondary font-bold py-2 px-2',
  ghost: 'border-transparent py-2 px-2',
  input: 'border px-4 py-2 rounded bg-primary border-primary',
};

type ContentAlignment = 'left' | 'center';
const contentAlignment: Record<ContentAlignment, string> = {
  left: 'justify-start',
  center: 'justify-center',
};

const textKinds: Record<ButtonVariant, TextKind> = {
  primary: 'bold',
  secondary: 'bold',
  input: 'normal',
  ghost: 'bold',
};

const textColors: Record<ButtonVariant, TextColor> = {
  primary: 'primary',
  secondary: 'secondary',
  input: 'primary',
  ghost: 'primary',
};

type ButtonProps = PropsWithChildren<TouchableOpacityProps> & {
  leftIcon?: React.ReactNode;
  variant: ButtonVariant;
  maxWidth?: boolean;
  rounded?: boolean;
  align?: 'left' | 'center';
};

export const Button: FC<ButtonProps> = ({
  children,
  leftIcon,
  variant,
  maxWidth = false,
  rounded = false,
  className,
  align = 'center',
  ...rest
}) => {
  const classNames: (string | undefined)[] = [
    'h-[46]',
    'flex-row',
    'items-center',
    buttonVariants[variant],
    contentAlignment[align],
  ];

  if (maxWidth) {
    classNames.push('w-full');
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
          variant="base1"
          kind={textKinds[variant]}
          color={textColors[variant]}
        >
          {children}
        </Text>
      ) : (
        children
      )}
    </TouchableOpacity>
  );
};
