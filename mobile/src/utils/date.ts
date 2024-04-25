import { formatISO } from 'date-fns';

// export function formatISODate(date: Date): string;
// export function formatISODate(date: undefined): undefined;
export function formatISODate(date: string | undefined): string | undefined {
  return date
    ? formatISO(new Date(date), { representation: 'date' })
    : undefined;
}
