import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_ffi_asset_helper/flutter_ffi_asset_helper.dart';
import 'package:flutter_ffi_asset_helper/flutter_ffi_asset_helper_platform_interface.dart';
import 'package:flutter_ffi_asset_helper/flutter_ffi_asset_helper_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockFlutterFfiAssetHelperPlatform 
    with MockPlatformInterfaceMixin
    implements FlutterFfiAssetHelperPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final FlutterFfiAssetHelperPlatform initialPlatform = FlutterFfiAssetHelperPlatform.instance;

  test('$MethodChannelFlutterFfiAssetHelper is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelFlutterFfiAssetHelper>());
  });

  test('getPlatformVersion', () async {
    FlutterFfiAssetHelper FlutterFfiAssetHelperPlugin = FlutterFfiAssetHelper();
    MockFlutterFfiAssetHelperPlatform fakePlatform = MockFlutterFfiAssetHelperPlatform();
    FlutterFfiAssetHelperPlatform.instance = fakePlatform;
  
    expect(await FlutterFfiAssetHelperPlugin.getPlatformVersion(), '42');
  });
}
