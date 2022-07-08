import 'dart:convert';
import 'dart:ffi';
import 'package:ffi_path_helper/ffi_path_helper_platform_interface.dart';
import 'package:flutter/material.dart';
import 'package:ffi_path_helper/ffi_path_helper.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final _ffiPathHelperPlugin = FfiPathHelper();

  FFIAsset? _asset;
  String? _contents;

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        home: Scaffold(
      appBar: AppBar(
        title: const Text('Plugin example app'),
      ),
      body: Center(
        child: Column(children: [
          ElevatedButton(
            child: const Text("Load"),
            onPressed: () async {
              _asset =
                  await _ffiPathHelperPlugin.assetToByteArrayPointer("assets/some_test_asset.txt");
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
                    await _ffiPathHelperPlugin.free(_asset!);
                  },
          ),
          ElevatedButton(
            child: const Text("Get file descriptor"),
            onPressed: () async {
              var path = await _ffiPathHelperPlugin.getFdFromAsset("assets/some_test_asset2.txt");
              await _ffiPathHelperPlugin.closeFd(path);
            },
          ),
        ]),
      ),
    ));
  }
}
