import 'ffi_path_helper_platform_interface.dart';

class FfiPathHelper {
  Future<FFIAsset> assetToByteArrayPointer(String assetPath) {
    return FfiPathHelperPlatform.instance.assetToByteArrayPointer(assetPath);
  }

  Future<String> getFdFromAsset(String assetPath) {
    return FfiPathHelperPlatform.instance.getFdFromAsset(assetPath);
  }

  Future closeFd(String assetPath) {
    return FfiPathHelperPlatform.instance.closeFd(assetPath);
  }

  Future free(FFIAsset asset) {
    return FfiPathHelperPlatform.instance.free(asset);
  }
}
