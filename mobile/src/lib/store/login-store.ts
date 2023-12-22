import { create } from 'zustand';

export const useLoginStore = create(() => ({
    isSignedIn: false,
}));
