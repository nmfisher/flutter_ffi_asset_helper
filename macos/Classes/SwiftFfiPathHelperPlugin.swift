import Cocoa
import FlutterMacOS

public class SwiftFfiPathHelperPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "ffi_path_helper", binaryMessenger: registrar.messenger)
    let instance = FfiPathHelperPlugin()
    registrar.addMethodCallDelegate(instance as! FlutterPlugin, channel: channel)
  }
  
}
