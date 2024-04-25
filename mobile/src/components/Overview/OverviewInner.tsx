import { Text } from '~/components/ui';

import { OverviewInnerContainer } from './OverviewInner.styles';

export const OverviewInner = () => {
  return (
    <OverviewInnerContainer>
      <Text variant="h1" kind="bold" className="mb-28">
        Overview
      </Text>
    </OverviewInnerContainer>
  );
};
