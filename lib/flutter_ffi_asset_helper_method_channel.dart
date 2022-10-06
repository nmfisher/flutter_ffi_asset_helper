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
  final methodChannel = const MethodChannel('app.polyvox/flutter_ffi_asset_helper');

  @override
  Future<FFIAsset> assetToByteArrayPointer(String path) async {
    if(Platform.isWindows) {
      final data = await rootBundle.load(path);
      final ptr = calloc.allocate<Uint8>(data.lengthInBytes);
      
      for(int i =0; i < data.lengthInBytes; i++) {
        ptr.elementAt(i).value = data.getUint8(i);
      }
      return FFIAsset(ptr.address, data.lengthInBytes, path);  
    } else {
      return await methodChannel.invokeMethod("assetToByteArrayPointer", path);
    }
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
    if(Platform.isWindows) {
      calloc.free(Pointer<Uint8>.fromAddress(asset.data));
    } else {
      if (!await methodChannel.invokeMethod("free", asset.path)) {
        throw Exception("Could not free FFI asset.");
      }
    }
  }
}