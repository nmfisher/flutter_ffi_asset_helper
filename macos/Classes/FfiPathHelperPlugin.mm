#import "FfiPathHelperPlugin.h"
#include <iostream>     
#include <fstream>      

using namespace std;

@implementation FfiPathHelperPlugin {
  NSObject<FlutterPluginRegistrar>* _registrar;
}

+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  FlutterMethodChannel* channel = [FlutterMethodChannel
      methodChannelWithName:@"app.polyvox/ffi_path_helper"
            binaryMessenger:[registrar messenger]];
  FfiPathHelperPlugin* instance = [[FfiPathHelperPlugin alloc] init];
  [registrar addMethodCallDelegate:instance channel:channel];
  instance->_registrar = registrar;
}

- (void)handleMethodCall:(FlutterMethodCall*)call result:(FlutterResult)result {

    if ([@"load" isEqualToString:call.method]) {
      NSLog(@"Loading asset at asset path %@", call.arguments);
      NSString* assetPath = call.arguments;
#if TARGET_OS_IPHONE
      NSString* key = [_registrar lookupKeyForAsset:assetPath];
      NSString* nsPath = [[NSBundle mainBundle] pathForResource:key ofType:nil];
      if(!nsPath) {
        NSLog(@"Couldn't find asset at path %@", key);
        exit(-1);
      }
#else
//      Bundle.main.bundlePath +  +
      NSString* nsPath = [NSString stringWithFormat:@"%@/Contents/Frameworks/App.framework/Resources/flutter_assets/%@", [[NSBundle mainBundle] bundlePath], assetPath];
      NSLog(@"Loading MacOS asset path %@", nsPath);

#endif

      
      ifstream is([nsPath fileSystemRepresentation], ios_base::binary);
      int length = 0;
      char* buffer;
      if (is) {
        is.seekg (0, is.end);
        length = is.tellg();
        NSLog(@"Asset length : %d", length);

        is.seekg (0, is.beg);
      
        buffer = new char [length];

        is.read (buffer,length);

        if (!is) {
          std::cout << "Read failed, only " << is.gcount() << " bytes read";
        }
        is.close();
      }
      // AFAIK the only way to return a pointer via platform channel with a standard codec is to cast to long
      id objects[] = { [NSNumber numberWithLong:(long)buffer], [NSNumber numberWithInt:length] };

      result([NSArray arrayWithObjects:objects count:2]);
      NSLog(@"Successfully loaded asset at %@", call.arguments);

  } else if([@"free" isEqualToString:call.method]) {
    NSLog(@"Freeing data at %lu", [call.arguments intValue]);

    free( (void*) [call.arguments longValue]);
    result([NSNumber numberWithBool:YES]);
  }
}
@end
