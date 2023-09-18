import Cocoa
import FlutterMacOS

public class FlutterFfiAssetHelperPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "app.polyvox/flutter_ffi_asset_helper", binaryMessenger: registrar.messenger)
    let instance = FlutterFfiAssetHelperPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  var _assets = [String:NSData]()

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
      case "assetToByteArrayPointer":
        print("Loading asset at asset path \(call.arguments!)");
        let bundle = Bundle.init(identifier: "io.flutter.flutter.app")!
        let path = bundle.path(forResource:call.arguments as! String, ofType: nil, inDirectory: "flutter_assets")
        if(path == nil) {
          print("File not present in bundle : \(call.arguments!)")
          result(nil)
        } else {
            do {
                let data = try Data(contentsOf: URL(fileURLWithPath:path!))
                let nsData = data as NSData
                _assets[call.arguments as! String] = nsData
                let ptr = Int(bitPattern:nsData.bytes)
                let length = nsData.length
                result([ptr, length])
            } catch {
                result(nil)
            }
        }
      case "free":
        _assets[call.arguments as! String] = nil
        result(true)
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
