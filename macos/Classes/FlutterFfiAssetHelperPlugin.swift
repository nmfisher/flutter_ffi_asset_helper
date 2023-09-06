import Cocoa
import FlutterMacOS

public class FlutterFfiAssetHelperPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "app.polyvox/flutter_ffi_asset_helper", binaryMessenger: registrar.messenger)
    let instance = FlutterFfiAssetHelperPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "assetToByteArrayPointer":
      result(FlutterMethodNotImplemented) 
    case "getFdFromAsset":
      print("Loading asset at asset path \(call.arguments)");
      let bundle = Bundle.init(identifier: "io.flutter.flutter.app")!
      let path = bundle.path(forResource:call.arguments as! String, ofType: nil, inDirectory: "flutter_assets")
      if(path == nil) {
        print("File not present in bundle : \(call.arguments)")
      }
      result(path);
    case "closeFd":
      // noop
      result(true)
    default:
      result(FlutterMethodNotImplemented) 
    }
  }
}
