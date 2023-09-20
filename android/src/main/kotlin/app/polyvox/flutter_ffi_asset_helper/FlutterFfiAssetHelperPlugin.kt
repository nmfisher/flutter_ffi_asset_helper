package app.polyvox.flutter_ffi_asset_helper

import androidx.annotation.NonNull

import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result

import  io.flutter.embedding.engine.loader.FlutterLoader
import io.flutter.FlutterInjector

import android.content.res.AssetManager

import android.content.Context
import android.os.Build
import android.system.Os
import android.os.ParcelFileDescriptor
import android.util.Log
import androidx.annotation.RequiresApi
import java.io.*

import java.nio.file.FileSystems
import java.nio.file.Files
import kotlin.io.path.absolutePathString
import kotlin.io.path.listDirectoryEntries
import kotlin.io.path.Path
import kotlin.io.path.createDirectories


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
  
  @Throws(IOException::class)
  private fun copyFile(`in`: InputStream?, out: OutputStream) {
    val buffer = ByteArray(1024)
    var read: Int? = null
    while (`in`?.read(buffer).also({ read = it!! }) != -1) {
        read?.let { out.write(buffer, 0, it) }
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
      val assetManager: AssetManager = context.assets

      var `in`: InputStream? = null
      var out: OutputStream? = null
      val outFile = File(context.getExternalFilesDir(null), call.arguments as String)

      Path(outFile.parent).createDirectories()
      try {
          `in` = assetManager.open(key)
          out = FileOutputStream(outFile)
          copyFile(`in`, out)
      } catch (e: IOException) {
          Log.e("tag", "Failed to copy asset file: ${call.arguments}", e)
      } finally {
          if (`in` != null) {
              try {
                  `in`.close()
              } catch (e: IOException) {
                  e.printStackTrace()
              }
          }
          if (out != null) {
              try {
                  out.close()
              } catch (e: IOException) {
                  e.printStackTrace()
              }
          }
      }
      result.success(outFile.path);
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
