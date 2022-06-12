import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'ffi_path_helper_method_channel.dart';

class FFIAsset {
  final int data;
  final int length;

  FFIAsset(this.data, this.length);
}

abstract class FfiPathHelperPlatform extends PlatformInterface {
  /// Constructs a FfiPathHelperPlatform.
  FfiPathHelperPlatform() : super(token: _token);

  static final Object _token = Object();

  static FfiPathHelperPlatform _instance = MethodChannelFfiPathHelper();

  /// The default instance of [FfiPathHelperPlatform] to use.
  ///
  /// Defaults to [MethodChannelFfiPathHelper].
  static FfiPathHelperPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [FfiPathHelperPlatform] when
  /// they register themselves.
  static set instance(FfiPathHelperPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<FFIAsset> load(String path);
  Future free(FFIAsset path);
}
