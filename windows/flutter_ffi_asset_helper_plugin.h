#ifndef FLUTTER_PLUGIN_FLUTTER_FFI_ASSET_HELPER_PLUGIN_H_
#define FLUTTER_PLUGIN_FLUTTER_FFI_ASSET_HELPER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace flutter_ffi_asset_helper {

class FlutterFfiAssetHelperPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  FlutterFfiAssetHelperPlugin();

  virtual ~FlutterFfiAssetHelperPlugin();

  // Disallow copy and assign.
  FlutterFfiAssetHelperPlugin(const FlutterFfiAssetHelperPlugin&) = delete;
  FlutterFfiAssetHelperPlugin& operator=(const FlutterFfiAssetHelperPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace flutter_ffi_asset_helper

#endif  // FLUTTER_PLUGIN_FLUTTER_FFI_ASSET_HELPER_PLUGIN_H_
