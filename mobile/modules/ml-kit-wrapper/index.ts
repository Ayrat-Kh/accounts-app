// Import the native module. On web, it will be resolved to MlKitWrapper.web.ts
// and on native platforms to MlKitWrapper.ts
import MlKitWrapperModule from "./src/MlKitWrapperModule";

export async function parseImage(iamgeUrl: string): Promise<any> {
  return await MlKitWrapperModule.parseImage(iamgeUrl);
}
