#include "include/flutter_ffi_asset_helper/flutter_ffi_asset_helper_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>
#include <math.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string> 
#include <map>
#include <unistd.h>

using namespace std;

#include <stdint.h>

#define FLUTTER_FFI_ASSET_HELPER_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), flutter_ffi_asset_helper_plugin_get_type(), \
                              FlutterFfiAssetHelperPlugin))

struct _FlutterFfiAssetHelperPlugin {
  GObject parent_instance;
  map<string, void*>* assets;
};

G_DEFINE_TYPE(FlutterFfiAssetHelperPlugin, flutter_ffi_asset_helper_plugin, g_object_get_type())

// Called when a method call is received from Flutter.
static void flutter_ffi_asset_helper_plugin_handle_method_call(
    FlutterFfiAssetHelperPlugin* self,
    FlMethodCall* method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar* method = fl_method_call_get_name(method_call);

  if (strcmp(method, "assetToByteArrayPointer") == 0) {
    
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
       std::cout << "Current working dir: " << cwd  << std::endl;
    }

    FlValue* args = fl_method_call_get_args(method_call);
    
    auto name = std::string(fl_value_get_string(args));
    
    // this functions accepts URIs, so  
    // - file:// points to a file on the filesystem
    // - asset:// points to an asset, usually resolved relative to the current working directory
    // - no prefix is presumed to be an asset
    if (name.rfind("file://", 0) == 0) {
      name = name.substr(7);
    } else if(name.rfind("asset://", 0) == 0) {
      name = name.substr(7);
      name = string(cwd) + string("/") + name;
    } else {
      name = string(cwd) + string("/build/linux/x64/debug/bundle/data/flutter_assets/") + name;
    }

    std::cout << "Loading resource at " << name.c_str() << std::endl;

 

    streampos length;
    ifstream is(name, ios::binary);
    if(!is) {
      std::cout << "Failed to find resource at file path " << name.c_str() << std::endl;
      response = FL_METHOD_RESPONSE(fl_method_error_response_new("ASSET_NOT_FOUND", "Asset not found", nullptr));
    } else {
      is.seekg (0, ios::end);
      length = is.tellg();
      char * buffer;
      buffer = new char [length];
      is.seekg (0, ios::beg);
      is.read (buffer, length);
      is.close();      
      
      auto id = self->assets->size();

      (*self->assets)[name] = buffer;  

      g_autoptr(FlValue) result = fl_value_new_list();

      fl_value_append_take (result, fl_value_new_int ((int64_t)buffer));
      fl_value_append_take (result, fl_value_new_int ((int64_t)length));
      fl_value_append_take (result, fl_value_new_int ((int64_t)id));
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
    }
  } else if (strcmp(method, "free") == 0) {
      FlValue* args = fl_method_call_get_args(method_call);
      auto id = string(fl_value_get_string(args));
      std::cout << "Freeing resource " << id << std::endl;
      auto it = self->assets->find(id);
      if (it != self->assets->end()) {
        delete((char*)it->second);
      }
      (*self->assets)[id] = nullptr;  
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_bool(true)));
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }
  
  fl_method_call_respond(method_call, response, nullptr);
}

static void flutter_ffi_asset_helper_plugin_dispose(GObject* object) {
  G_OBJECT_CLASS(flutter_ffi_asset_helper_plugin_parent_class)->dispose(object);
}

static void flutter_ffi_asset_helper_plugin_class_init(FlutterFfiAssetHelperPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = flutter_ffi_asset_helper_plugin_dispose;
}

static void flutter_ffi_asset_helper_plugin_init(FlutterFfiAssetHelperPlugin* self) {
  if(!self->assets) {
    self->assets = new map<string, void*>();
  }
}

static void method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  FlutterFfiAssetHelperPlugin* plugin = FLUTTER_FFI_ASSET_HELPER_PLUGIN(user_data);
  flutter_ffi_asset_helper_plugin_handle_method_call(plugin, method_call);
}

void flutter_ffi_asset_helper_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  FlutterFfiAssetHelperPlugin* plugin = FLUTTER_FFI_ASSET_HELPER_PLUGIN(
      g_object_new(flutter_ffi_asset_helper_plugin_get_type(), nullptr));

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "app.polyvox/flutter_ffi_asset_helper",
                            FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  g_object_unref(plugin);
}
