import { useState } from 'react';

type UseModalResult = {
  isVisible: boolean;
  setVisiblity: (isVisible: boolean) => void;
  toggleVisibility: VoidFunction;
  show: VoidFunction;
  close: VoidFunction;
};

export const useModal = (isVisibleInital = false): UseModalResult => {
  const [isVisible, setVisiblity] = useState(isVisibleInital);

  const toggleVisibility = () => {
    setVisiblity((prev) => !prev);
  };

  const show = () => {
    setVisiblity(true);
  };

  const close = () => {
    setVisiblity(false);
  };

  return { isVisible, setVisiblity, toggleVisibility, show, close };
};
