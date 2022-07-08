package com.example.ffi_path_helper

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
import android.os.ParcelFileDescriptor
import android.util.Log

import com.sun.jna.Library
import com.sun.jna.Native
import com.sun.jna.Pointer
import com.sun.jna.Structure
import com.sun.jna.NativeLibrary
import com.sun.jna.StringArray
import android.R.attr.path

import android.net.LocalSocket
import android.net.LocalSocketAddress
import java.io.*

/** FfiPathHelperPlugin */
class FfiPathHelperPlugin: FlutterPlugin, MethodCallHandler {
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
    channel = MethodChannel(flutterPluginBinding.binaryMessenger, "app.polyvox/ffi_path_helper")
    channel.setMethodCallHandler(this)
    context = flutterPluginBinding.applicationContext
  }

  
  companion object {
    init {
      System.loadLibrary("ffi_path_helper");
    }
  }

  override fun onMethodCall(@NonNull call: MethodCall, @NonNull result: Result) {
    if (call.method == "assetToByteArrayPointer") {
      val loader = FlutterInjector.instance().flutterLoader()
      val extDir = context.getExternalFilesDir(null)
      val key = loader.getLookupKeyForAsset(call.arguments as String)
      val path = extDir!!.path + "/" + key
      val ptr = getNativePointer(context.getAssets(), key)
      result.success(ptr);
    } else if(call.method == "free") {
      val loader = FlutterInjector.instance().flutterLoader()
      val extDir = context.getExternalFilesDir(null)
      val key = loader.getLookupKeyForAsset(call.arguments as String)
      val path = extDir!!.path + "/" + key
      free(context.getAssets(), key)
    } else if(call.method == "getFdFromAsset") {
      val loader = FlutterInjector.instance().flutterLoader()
      val extDir = context.getExternalFilesDir(null)
      val key = loader.getLookupKeyForAsset(call.arguments as String)
      val path = extDir!!.path + "/" + key
      val fd = getFdFromAsset(context.getAssets(), key)
      result.success("/proc/self/fd/${fd}");
    } else if(call.method == "closeFd") {
      val parts = (call.arguments as String).split("/")
      closeFd(parts.last().toInt())
    } else {
      result.notImplemented()
    }
  }

  override fun onDetachedFromEngine(@NonNull binding: FlutterPlugin.FlutterPluginBinding) {
    channel.setMethodCallHandler(null)
  }
}
