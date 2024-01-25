import type { FC, PropsWithChildren } from 'react';
import { Text as TextRN, type TextProps as TextRNProps } from 'react-native';

type TextProps = PropsWithChildren<
  TextRNProps & {
    variant?:
      | 'h1'
      | 'h2'
      | 'h3'
      | 'body1'
      | 'body2'
      | 'base1'
      | 'base2'
      | 'base3';
    color?: 'primary' | 'primary-low' | 'secondary' | 'secondary-low';
    kind?: 'normal' | 'semibold' | 'bold';
  }
>;

export const Text: FC<TextProps> = ({
  variant = 'base1',
  color = 'primary',
  kind = 'normal',
  className,
  children,
  ...rest
}: TextProps) => {
  const classNames = [];

  switch (variant) {
    case 'h1': {
      classNames.push('text-5xl'); // 48
      break;
    }
    case 'h2': {
      classNames.push('text-4xl'); // 36
      break;
    }
    case 'h3': {
      classNames.push('text-3xl'); // 30
      break;
    }
    case 'body1': {
      classNames.push('text-xl'); // 20
      break;
    }
    case 'body2': {
      classNames.push('text-lg'); // 18
      break;
    }
    case 'base1': {
      classNames.push('text-base'); // 16
      break;
    }
    case 'base2': {
      classNames.push('text-sm'); // 14
      break;
    }
    case 'base3': {
      classNames.push('text-xs'); // 12
      break;
    }
  }

  switch (kind) {
    case 'normal': {
      classNames.push('font-normal'); // 26
      break;
    }
    case 'bold': {
      classNames.push('font-bold'); // 24
      break;
    }
    case 'semibold': {
      classNames.push('font-semibold'); // 24
      break;
    }
  }

  switch (color) {
    case 'primary': {
      classNames.push('text-app-primary-1200'); // 26
      break;
    }
    case 'primary-low': {
      classNames.push('text-app-primary-1100'); // 26
      break;
    }
    case 'secondary': {
      classNames.push('text-app-secondary-1200'); // 24
      break;
    }
    case 'secondary-low': {
      classNames.push('text-app-secondary-1100'); // 24
      break;
    }
  }

  classNames.push(className);

  return (
    <TextRN {...rest} className={classNames.join(' ')}>
      {children}
    </TextRN>
  );
};
