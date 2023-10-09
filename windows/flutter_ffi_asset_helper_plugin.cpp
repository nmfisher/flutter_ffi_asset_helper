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
#include <codecvt>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <locale>

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
  
  // this is just a rough outline for mmap-ing the requested asset/file and returning a pointer to the start of the mapped memory
  // will throw an error because it wasn't ever actually checked - this is just a skeleton, if you want to implement, test it properly.
  if (method_call.method_name() == "assetToByteArrayPointer") {

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
    // result->Success(flutter::EncodableValue(buf));
    result->NotImplemented();
  } else if (method_call.method_name() == "free") {
    // same as above - need to implement free logic
    result->NotImplemented();
  } else if (method_call.method_name() == "assetToFilepath") {

    const auto *assetPath = std::get_if<std::string>(method_call.arguments());
    std::cout << "Returning filesystem path to asset : " << *assetPath << std::endl;
    TCHAR pBuf[256];
    size_t len = sizeof(pBuf);
    int bytes = GetModuleFileName(NULL, pBuf, (DWORD)len);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wAssetPath = converter.from_bytes(*assetPath);

    std::wstring exePathBuf(pBuf);
    std::filesystem::path exePath(exePathBuf);
    auto exeDir = exePath.remove_filename();
    std::filesystem::path targetFilePath = exeDir.wstring() + L"data/flutter_assets/" +
                            wAssetPath;
    result->Success(flutter::EncodableValue(targetFilePath.u8string()));
  } else if (method_call.method_name() == "closeFile") {
    // noop
    result->Success(flutter::EncodableValue(true));
  } else {
    result->NotImplemented();
  }
}

} // namespace flutter_ffi_asset_helper
