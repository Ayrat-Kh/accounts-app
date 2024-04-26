import {
  type FC,
  type PropsWithChildren,
  type ReactElement,
  cloneElement,
} from 'react';
import {
  TouchableHighlight,
  type TouchableHighlightProps, 
  View,
} from 'react-native';

import { IconColor } from '~/assets/icons/types';
import { AppBgColorKey, useBgColor } from '~/ui';

import { Text, TextColor, type TextKind } from './Text';
import { type InputSizeVariant, inputVariantSize } from './utils';

type ButtonVariant = 'primary' | 'secondary' | 'ghost' | 'input';
const buttonVariants: Record<ButtonVariant, string> = {
  primary: 'bg-compPrimary border-2 border-primary font-bold py-2 px-2',
  secondary: 'bg-compSecondary border-2 border-secondary font-bold py-2 px-2',
  ghost: 'border-transparent py-2 px-2',
  input: 'border px-4 py-2 rounded bg-surface border-secondary',
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
  input: 'secondary',
  ghost: 'primary',
};

const underlayColors: Record<ButtonVariant, AppBgColorKey> = {
  primary: 'compPrimary',
  secondary: 'compSecondary',
  input: 'compSecondary',
  ghost: 'compPrimary',
};

const iconColors: Record<ButtonVariant, IconColor> = {
  ghost: 'primary',
  input: 'primary',
  primary: 'primary',
  secondary: 'secondary',
};

type ButtonProps = (
  | PropsWithChildren<TouchableHighlightProps>
  | (TouchableHighlightProps & { icon?: ReactElement })
) & {
  leftIcon?: ReactElement;
  variant: ButtonVariant;
  size?: InputSizeVariant;
  maxWidth?: boolean;
  rounded?: boolean;
  align?: 'left' | 'center';
};

export const Button: FC<ButtonProps> = ({
  leftIcon,
  variant,
  size = 'md',
  maxWidth = false,
  rounded = false,
  className,
  align = 'center',
  ...rest
}) => {
  const classNames: (string | undefined)[] = [
    'flex-row',
    'items-center',
    buttonVariants[variant],
    contentAlignment[align],
    inputVariantSize[size],
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

  const primaryBackColor = useBgColor(underlayColors[variant]);

  // set icon color based on button variant
  const centralComponent =
    'icon' in rest && rest.icon
      ? cloneElement(rest.icon, {
          color: iconColors[variant],
        })
      : rest.children;

  return (
    <TouchableHighlight
      underlayColor={primaryBackColor}
      {...rest}
      className={classNames.join(' ')}
    >
      <View>
        {leftIcon}

        {Boolean(leftIcon && centralComponent) && <View className="mr-2" />}

        {typeof centralComponent === 'string' ? (
          <Text
            variant="base1"
            kind={textKinds[variant]}
            color={textColors[variant]}
          >
            {centralComponent}
          </Text>
        ) : (
          centralComponent
        )}
      </View>
    </TouchableHighlight>
  );
};
