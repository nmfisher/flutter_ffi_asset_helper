#include "include/flutter_ffi_asset_helper/flutter_ffi_asset_helper_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "flutter_ffi_asset_helper_plugin.h"

void FlutterFfiAssetHelperPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  flutter_ffi_asset_helper::FlutterFfiAssetHelperPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
