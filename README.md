# flutter_ffi_asset_helper

When using FFI to invoke native (C) functions from Dart, we often want to pass some kind of buffer/pointer to a packaged asset.

The "easy" way is to use `rootBundle.load` and then copy into a `Pointer` which is passed via FFI. There are two obvious problems with this:
1) this requires a copy, which can be quite slow for large assets,
2) this requires allocation on the Dart GC heap.

These are both unnecessary, which this plugin addresses by exposing various functions for opening packaged assets and working directly with void* pointers.

This plugin also lets you convert assets to file descriptors in the form of named pipes on Android (created via mkfifo) - this is not required on iOS as all assets can be accessed as regular files). However this is buggy and I wouldn't advise using it.

