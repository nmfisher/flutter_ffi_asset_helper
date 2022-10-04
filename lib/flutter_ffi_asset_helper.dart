import 'flutter_ffi_asset_helper_platform_interface.dart';

class FlutterFfiAssetHelper {
  Future<FFIAsset> assetToByteArrayPointer(String assetPath) {
    return FlutterFfiAssetHelperPlatform.instance.assetToByteArrayPointer(assetPath);
  }

  Future<String> getFdFromAsset(String assetPath) {
    return FlutterFfiAssetHelperPlatform.instance.getFdFromAsset(assetPath);
  }

  Future closeFd(String assetPath) {
    return FlutterFfiAssetHelperPlatform.instance.closeFd(assetPath);
  }

  Future free(FFIAsset asset) {
    return FlutterFfiAssetHelperPlatform.instance.free(asset);
  }
}
