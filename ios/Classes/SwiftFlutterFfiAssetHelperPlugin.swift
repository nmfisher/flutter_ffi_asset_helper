import Flutter
import UIKit

public class SwiftFlutterFfiAssetHelperPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "flutter_ffi_asset_helper", binaryMessenger: registrar.messenger())
    let instance = SwiftFlutterFfiAssetHelperPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    result("iOS " + UIDevice.current.systemVersion)
  }
}
