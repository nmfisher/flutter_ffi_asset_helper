#import "FlutterFfiAssetHelperPlugin.h"
#include <iostream>     
#include <fstream>      
#include <map>

using namespace std;

static map<string, AAsset*> assets;

@implementation FlutterFfiAssetHelperPlugin {
  NSObject<FlutterPluginRegistrar>* _registrar;
}

+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  FlutterMethodChannel* channel = [FlutterMethodChannel
      methodChannelWithName:@"app.polyvox/flutter_ffi_asset_helper"
            binaryMessenger:[registrar messenger]];
  FlutterFfiAssetHelperPlugin* instance = [[FlutterFfiAssetHelperPlugin alloc] init];
  [registrar addMethodCallDelegate:instance channel:channel];
  instance->_registrar = registrar;
}

- (void)handleMethodCall:(FlutterMethodCall*)call result:(FlutterResult)result {

    if ([@"assetToByteArrayPointer" isEqualToString:call.method]) {
      NSLog(@"Loading asset at asset path %@", call.arguments);
      NSString* assetPath = call.arguments; 
      NSString* key = [_registrar lookupKeyForAsset:assetPath];
      NSString* nsPath = [[NSBundle mainBundle] pathForResource:key ofType:nil];
      if(!nsPath) {
        NSLog(@"Couldn't find asset at path %@", key);
        exit(-1);
      }
      
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

      assets.insert({string([nsPath UTF8String]), buffer});

      result([NSArray arrayWithObjects:objects count:2]);
      NSLog(@"Successfully loaded asset at %@", call.arguments);

  } else if([@"free" isEqualToString:call.method]) {
    NSLog(@"Freeing data for path %@", call.arguments);

    free( (void*) [call.arguments longValue]);
    result([NSNumber numberWithBool:YES]);
  }
}
@end
