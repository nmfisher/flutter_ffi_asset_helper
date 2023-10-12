import 'dart:ffi';
import 'dart:io';
import 'package:ffi/ffi.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'flutter_ffi_asset_helper_platform_interface.dart';

/// An implementation of [FlutterFfiAssetHelperPlatform] that uses method channels.
class MethodChannelFlutterFfiAssetHelper extends FlutterFfiAssetHelperPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel =
      const MethodChannel('app.polyvox/flutter_ffi_asset_helper');

  @override
  Future<FFIAsset> assetToByteArrayPointer(String path) async {
    if (Platform.isWindows) {
      final data = await rootBundle.load(path);
      final ptr = calloc.allocate<Uint8>(data.lengthInBytes);

      for (int i = 0; i < data.lengthInBytes; i++) {
        ptr.elementAt(i).value = data.getUint8(i);
      }
      return FFIAsset(ptr.address, data.lengthInBytes, path);
    } else {
      var result =
          await methodChannel.invokeMethod("assetToByteArrayPointer", path);
      return FFIAsset(result[0], result[1], path);
    }
  }

  @override
  Future<String> assetToFilepath(String path) async {
    var result = await methodChannel.invokeMethod("assetToFilepath", path);
    if (result == null) {
      throw Exception(
          "Failed to find filepath for asset : $path (current directory : ${Directory.current.path})");
    }
    return result as String;
  }

  @override
  Future closeFile(String path) async {
    return methodChannel.invokeMethod("closeFile", path);
  }

  @override
  Future free(FFIAsset asset) async {
    if (asset.released) {
      throw Exception(
          "Asset has already been released. You shouldn't have held on to this reference");
    }
    if (Platform.isWindows) {
      calloc.free(Pointer<Uint8>.fromAddress(asset.data));
    } else {
      var result = await methodChannel.invokeMethod("free", asset.path);
      if (!result) {
        throw Exception("Could not free FFI asset.");
      }
    }
    asset.released = true;
  }
}
