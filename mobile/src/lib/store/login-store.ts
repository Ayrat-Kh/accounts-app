import { create } from 'zustand';
import { combine } from 'zustand/middleware';

export const useLoginStore = create(
    combine(
        {
            isSignedIn: false,
        },
        () => ({})
    )
);
