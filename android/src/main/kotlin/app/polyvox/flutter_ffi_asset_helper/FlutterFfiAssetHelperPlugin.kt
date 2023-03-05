package app.polyvox.flutter_ffi_asset_helper

import androidx.annotation.NonNull

import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result

import  io.flutter.embedding.engine.loader.FlutterLoader
import io.flutter.FlutterInjector

import java.io.FileReader
import java.io.BufferedReader
import java.io.InputStream
import java.io.InputStreamReader
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.FileNotFoundException

import android.content.res.AssetManager

import java.io.File

import android.content.Context
import android.os.Build
import android.os.ParcelFileDescriptor
import android.util.Log
import androidx.annotation.RequiresApi

import java.io.*
import java.nio.file.FileSystems
import java.nio.file.Files
import java.nio.file.Path
import kotlin.io.path.absolutePathString
import kotlin.io.path.listDirectoryEntries

/** FlutterFfiAssetHelperPlugin */
class FlutterFfiAssetHelperPlugin: FlutterPlugin, MethodCallHandler {
  /// The MethodChannel that will the communication between Flutter and native Android
  ///
  /// This local reference serves to register the plugin with the Flutter Engine and unregister it
  /// when the Flutter Engine is detached from the Activity
  private lateinit var channel : MethodChannel

  private lateinit var context: Context

  external fun getNativePointer(mgr: AssetManager, path: String) : LongArray;
  external fun free(mgr: AssetManager, path: String);
  external fun getFdFromAsset(mgr: AssetManager, path: String) : Int;
  external fun closeFd(fd: Int);

  override fun onAttachedToEngine(@NonNull flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
    channel = MethodChannel(flutterPluginBinding.binaryMessenger, "app.polyvox/flutter_ffi_asset_helper")
    channel.setMethodCallHandler(this)
    context = flutterPluginBinding.applicationContext
  }

  
  companion object {
    init {
      System.loadLibrary("flutter_ffi_asset_helper");
    }
  }

  override fun onMethodCall(@NonNull call: MethodCall, @NonNull result: Result) {
    if (call.method == "assetToByteArrayPointer") {
      val loader = FlutterInjector.instance().flutterLoader()
      val key = loader.getLookupKeyForAsset(call.arguments as String)
      val ptr = getNativePointer(context.getAssets(), key)
      result.success(ptr);
    } else if(call.method == "free") {
      val loader = FlutterInjector.instance().flutterLoader()
      val key = loader.getLookupKeyForAsset(call.arguments as String)
      free(context.getAssets(), key)
      result.success(true);
    } else if(call.method == "getFdFromAsset") {
      val loader = FlutterInjector.instance().flutterLoader()
      val key = loader.getLookupKeyForAsset(call.arguments as String)
      val fd = getFdFromAsset(context.getAssets(), key)
      result.success("/proc/self/fd/${fd}");
    } else if(call.method == "closeFd") {
      val parts = (call.arguments as String).split("/")
      closeFd(parts.last().toInt())
      result.success(true);
    } else {
      result.notImplemented()
    }
  }

  override fun onDetachedFromEngine(@NonNull binding: FlutterPlugin.FlutterPluginBinding) {
    channel.setMethodCallHandler(null)
  }
}
