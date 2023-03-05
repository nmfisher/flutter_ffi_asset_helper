import 'dart:ffi';

import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'flutter_ffi_asset_helper_method_channel.dart';

class FFIAsset {
  final int data;
  final int length;
  final String path;

  FFIAsset(this.data, this.length, this.path);
}

abstract class FlutterFfiAssetHelperPlatform extends PlatformInterface {
  /// Constructs a FlutterFfiAssetHelperPlatform.
  FlutterFfiAssetHelperPlatform() : super(token: _token);

  static final Object _token = Object();

  static FlutterFfiAssetHelperPlatform _instance =
      MethodChannelFlutterFfiAssetHelper();

  /// The default instance of [FlutterFfiAssetHelperPlatform] to use.
  ///
  /// Defaults to [MethodChannelFlutterFfiAssetHelper].
  static FlutterFfiAssetHelperPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [FlutterFfiAssetHelperPlatform] when
  /// they register themselves.
  static set instance(FlutterFfiAssetHelperPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<FFIAsset> assetToByteArrayPointer(String path);
  Future<String> getFdFromAsset(String path);
  Future closeFd(String path);
  Future free(FFIAsset path);
}
