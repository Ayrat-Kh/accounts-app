import { formatISO } from 'date-fns';

// export function formatISODate(date: Date): string;
// export function formatISODate(date: undefined): undefined;
export function formatISODate(date: Date | undefined): string | undefined {
  return date ? formatISO(date, { representation: 'date' }) : undefined;
}
