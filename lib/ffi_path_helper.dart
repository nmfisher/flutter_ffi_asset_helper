import 'ffi_path_helper_platform_interface.dart';

class FfiPathHelper {
  Future<FFIAsset> load(String assetPath) {
    return FfiPathHelperPlatform.instance.load(assetPath);
  }

  Future free(FFIAsset asset) {
    return FfiPathHelperPlatform.instance.free(asset);
  }
}
