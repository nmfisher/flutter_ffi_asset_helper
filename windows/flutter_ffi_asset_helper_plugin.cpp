#include "flutter_ffi_asset_helper_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include <MemoryApi.h>
#include <direct.h>
#define GetCurrentDir _getcwd

namespace flutter_ffi_asset_helper {

// static
void FlutterFfiAssetHelperPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "app.polyvox/flutter_ffi_asset_helper",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<FlutterFfiAssetHelperPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

FlutterFfiAssetHelperPlugin::FlutterFfiAssetHelperPlugin() {}

FlutterFfiAssetHelperPlugin::~FlutterFfiAssetHelperPlugin() {}

void FlutterFfiAssetHelperPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("assetToByteArrayPointer") == 0) {

    // mmap the requested file
    const auto *path = std::get_if<std::string>(method_call.arguments());
    char buff[FILENAME_MAX]; // create string buffer to hold path
    GetCurrentDir(buff, FILENAME_MAX);
    std::cout << "Opening " << path->c_str() << " with cwd " << buff
              << std::endl;
    HANDLE file =
        CreateFile((LPCWSTR)path->c_str(), GENERIC_READ, FILE_SHARE_READ, 0,
                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) {
      // TODO
      result->NotImplemented();
      return;
    };
    HANDLE map_handle =
        CreateFileMapping(file, NULL, PAGE_READWRITE | SEC_RESERVE, 0, 0, 0);
    if (map_handle == NULL) {
      // TODO
      CloseHandle(file);
      result->NotImplemented();
      return;
    }

    char *buf = (char *)MapViewOfFile(map_handle, FILE_MAP_READ, 0, 0, 0);

    if (buf == NULL) {
      CloseHandle(map_handle);
      CloseHandle(file);
      result->Error("FAILED", "Empty file?", flutter::EncodableValue(""));
      return;
    }
    result->Success(flutter::EncodableValue(buf));
  } else if (method_call.method_name().compare("free") == 0) {
    
  } else {
    result->NotImplemented();
  }
}

} // namespace flutter_ffi_asset_helper
