import 'dart:convert';
import 'dart:ffi';
import 'dart:io';
import 'package:permission_handler/permission_handler.dart';
import 'package:flutter_ffi_asset_helper/flutter_ffi_asset_helper_platform_interface.dart';
import 'package:flutter/material.dart';
import 'package:flutter_ffi_asset_helper/flutter_ffi_asset_helper.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        home: Scaffold(
            appBar: AppBar(
              title: const Text('Plugin example app'),
            ),
            body: Body()));
  }
}

class Body extends StatefulWidget {
  @override
  State<StatefulWidget> createState() => BodyState();
}

class BodyState extends State<Body> {
  final _FlutterFfiAssetHelperPlugin = FlutterFfiAssetHelper();

  FFIAsset? _asset;
  String? _contents;

  @override
  void initState() {
    super.initState();
    Permission.storage.request();
    Permission.manageExternalStorage.request();
  }

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(children: [
        ElevatedButton(
          child: const Text("Load"),
          onPressed: () async {
            // var ptr = ImmutableBuffer.fromAsset("assets/some_test_asset.txt");
            _asset = await _FlutterFfiAssetHelperPlugin.assetToByteArrayPointer(
                "assets/some_test_asset.txt");
            setState(() {
              var ptr = Pointer<Char>.fromAddress(_asset!.data);
              var data = List.generate(
                  _asset!.length, (index) => ptr.elementAt(index).value);
              _contents = Utf8Decoder().convert(data);
            });
          },
        ),
        _contents == null ? Container() : Text(_contents!),
        ElevatedButton(
          child: const Text("Free"),
          onPressed: _asset == null
              ? null
              : () async {
                  await _FlutterFfiAssetHelperPlugin.free(_asset!);
                },
        ),
        ElevatedButton(
          child: const Text("Get file descriptor"),
          onPressed: () async {
            var path = await _FlutterFfiAssetHelperPlugin.assetToFilepath(
                "assets/some_test_asset2.txt");

            await showDialog(
                builder: (ctx) {
                  return Center(
                      child: Container(
                          color: Colors.white,
                          child: Column(children: [
                            Text(path),
                            Text(File(path).readAsStringSync().toString())
                          ])));
                },
                context: context);
            await _FlutterFfiAssetHelperPlugin.closeFile(path);
          },
        ),
      ]),
    );
  }
}
