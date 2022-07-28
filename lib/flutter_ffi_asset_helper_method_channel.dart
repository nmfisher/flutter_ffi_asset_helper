import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'flutter_ffi_asset_helper_platform_interface.dart';

/// An implementation of [FlutterFfiAssetHelperPlatform] that uses method channels.
class MethodChannelFlutterFfiAssetHelper extends FlutterFfiAssetHelperPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('app.polyvox/flutter_ffi_asset_helper');

  @override
  Future<FFIAsset> assetToByteArrayPointer(String path) async {
    final result = await methodChannel.invokeMethod("assetToByteArrayPointer", path);
    return FFIAsset(result[0], result[1], path);
  }

  @override
  Future<String> getFdFromAsset(String path) async {
    var result = await methodChannel.invokeMethod("getFdFromAsset", path);
    return result as String;
  }

  @override
  Future closeFd(String path) async {
    return methodChannel.invokeMethod("closeFd", path);
  }

  @override
  Future free(FFIAsset asset) async {
    if (!await methodChannel.invokeMethod("free", asset.path)) {
      throw Exception("Could not free FFI asset.");
    }
  }
}
