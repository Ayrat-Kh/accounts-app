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
  normal: 'font-normal font-app',
  bold: 'font-bold font-app',
  semibold: 'font-semibold font-app',
};

export type TextColor =
  | 'primary'
  | 'primary-low'
  | 'secondary'
  | 'secondary-low';

export const textColors: Record<TextColor, string> = {
  primary: 'text-primary',
  'primary-low': 'text-primaryLow',
  secondary: 'text-secondary',
  'secondary-low': 'text-secondaryLow',
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
