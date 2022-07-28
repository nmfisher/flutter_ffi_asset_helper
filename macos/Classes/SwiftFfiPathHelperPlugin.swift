import Cocoa
import FlutterMacOS

public class SwiftFlutterFfiAssetHelperPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "flutter_ffi_asset_helper", binaryMessenger: registrar.messenger)
    let instance = FlutterFfiAssetHelperPlugin()
    registrar.addMethodCallDelegate(instance as! FlutterPlugin, channel: channel)
  }
  
}
