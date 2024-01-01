import { Text, View } from 'react-native';

import { Button } from '~/components/ui/Button';

export const Login = () => {
  return (
    <View>
      <Text className=" text-gray-50">Google login</Text>
      <Button>
        <Text>Google login</Text>
      </Button>
    </View>
  );
};
