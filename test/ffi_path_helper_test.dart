import 'package:flutter_test/flutter_test.dart';
import 'package:ffi_path_helper/ffi_path_helper.dart';
import 'package:ffi_path_helper/ffi_path_helper_platform_interface.dart';
import 'package:ffi_path_helper/ffi_path_helper_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockFfiPathHelperPlatform 
    with MockPlatformInterfaceMixin
    implements FfiPathHelperPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final FfiPathHelperPlatform initialPlatform = FfiPathHelperPlatform.instance;

  test('$MethodChannelFfiPathHelper is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelFfiPathHelper>());
  });

  test('getPlatformVersion', () async {
    FfiPathHelper ffiPathHelperPlugin = FfiPathHelper();
    MockFfiPathHelperPlatform fakePlatform = MockFfiPathHelperPlatform();
    FfiPathHelperPlatform.instance = fakePlatform;
  
    expect(await ffiPathHelperPlugin.getPlatformVersion(), '42');
  });
}
