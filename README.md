# flutter_ffi_asset_helper

A Flutter plugin to open assets as either:
- raw (char pointer) buffers 
- named pipes created via mkfifo (Android only, this is not required on iOS as all assets can be accessed as regular files).

This is generally used when invoking native functions/libraries via FFI that (on Android) require filepaths or that (on Android/iOS) require a pointer to a raw buffer/stream.

## Getting Started

This project is a starting point for a Flutter
[plug-in package](https://flutter.dev/developing-packages/),
a specialized package that includes platform-specific implementation code for
Android and/or iOS.

For help getting started with Flutter development, view the
[online documentation](https://flutter.dev/docs), which offers tutorials,
samples, guidance on mobile development, and a full API reference.

