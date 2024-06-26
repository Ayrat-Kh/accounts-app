import { useGetMe } from '~/lib/api/authUser';
import { useLoginStore } from '~/lib/store';

export const useLogoutWatcher = () => {
  const accessToken = useLoginStore((state) => state.accessToken);

  const { isError } = useGetMe({
    enabled: Boolean(accessToken),
  });

  if (Boolean(accessToken) && isError) {
    useLoginStore.getState().logout();
  }
};
