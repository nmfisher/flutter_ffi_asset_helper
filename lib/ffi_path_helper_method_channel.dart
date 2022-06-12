import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'ffi_path_helper_platform_interface.dart';

/// An implementation of [FfiPathHelperPlatform] that uses method channels.
class MethodChannelFfiPathHelper extends FfiPathHelperPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('app.polyvox/ffi_path_helper');

  @override
  Future<FFIAsset> load(String path) async {
    final result = await methodChannel.invokeMethod("load", path);
    return FFIAsset(result[0], result[1]);
  }

  @override
  Future free(FFIAsset asset) async {
    if (!await methodChannel.invokeMethod("free", asset.data)) {
      throw Exception("Could not free FFI asset data pointer");
    }
  }
}
