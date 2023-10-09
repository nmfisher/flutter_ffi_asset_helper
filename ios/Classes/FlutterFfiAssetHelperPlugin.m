#import "FlutterFfiAssetHelperPlugin.h"
#if __has_include(<flutter_ffi_asset_helper/flutter_ffi_asset_helper-Swift.h>)
#import <flutter_ffi_asset_helper/flutter_ffi_asset_helper-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "flutter_ffi_asset_helper-Swift.h"
#endif

@implementation FlutterFfiAssetHelperPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftFlutterFfiAssetHelperPlugin registerWithRegistrar:registrar];
}
@end
