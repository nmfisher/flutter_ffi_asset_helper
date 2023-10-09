import 'flutter_ffi_asset_helper_platform_interface.dart';

class FlutterFfiAssetHelper {
  Future<FFIAsset> assetToByteArrayPointer(String assetPath) {
    return FlutterFfiAssetHelperPlatform.instance.assetToByteArrayPointer(assetPath);
  }

  Future<String> assetToFilepath(String assetPath) {
    return FlutterFfiAssetHelperPlatform.instance.assetToFilepath(assetPath);
  }

  Future closeFile(String assetPath) {
    return FlutterFfiAssetHelperPlatform.instance.closeFile(assetPath);
  }

  Future free(FFIAsset asset) {
    return FlutterFfiAssetHelperPlatform.instance.free(asset);
  }
}
