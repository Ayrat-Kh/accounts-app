import type { FC, PropsWithChildren } from 'react';
import { Text as TextRN, type TextProps as TextRNProps } from 'react-native';

export type TextVariant =
  | 'h1'
  | 'h2'
  | 'h3'
  | 'body1'
  | 'body2'
  | 'base1'
  | 'base2'
  | 'base3';

const textVariants: Record<TextVariant, string> = {
  h1: 'text-5xl', // 48
  h2: 'text-4xl', // 36
  h3: 'text-3xl', // 30
  body1: 'text-xl', // 20
  body2: 'text-lg', // 18
  base1: 'text-base', // 16
  base2: 'text-sm', // 14
  base3: 'text-xs', // 12
};

export type TextKind = 'normal' | 'semibold' | 'bold';

const textKinds: Record<TextKind, string> = {
  normal: 'font-normal',
  bold: 'font-bold',
  semibold: 'font-semibold',
};

export type TextColor =
  | 'primary'
  | 'primary-low'
  | 'secondary'
  | 'secondary-low';

export const textColors: Record<TextColor, string> = {
  primary: 'font-normal',
  'primary-low': 'font-bold',
  secondary: 'font-semibold',
  'secondary-low': 'font-semibold',
};

type TextProps = PropsWithChildren<
  TextRNProps & {
    variant?: TextVariant;
    color?: TextColor;
    kind?: TextKind;
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
  const classNames: (string | undefined)[] = [
    textKinds[kind],
    textVariants[variant],
    textColors[color],
    className,
  ];

  return (
    <TextRN {...rest} className={classNames.join(' ')}>
      {children}
    </TextRN>
  );
};
