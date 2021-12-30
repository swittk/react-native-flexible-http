#import "FlexibleHttp.h"
#import "react-native-flexible-http.h"
#import <React/RCTUtils.h>
#import <React/RCTBridge+Private.h>
#import <jsi/jsi.h>
/*
 * Example of initialization from https://github.com/ammarahm-ed/react-native-jsi-template/blob/master/ios/SimpleJsi.mm
 */
@implementation FlexibleHttp
@synthesize bridge = _bridge;


RCT_EXPORT_MODULE()

// Example method for C++
// See the implementation of the example module in the `cpp` folder
RCT_EXPORT_METHOD(multiply:(nonnull NSNumber*)a withB:(nonnull NSNumber*)b
                  withResolver:(RCTPromiseResolveBlock)resolve
                  withReject:(RCTPromiseRejectBlock)reject)
{
    NSNumber *result = @(example::multiply([a floatValue], [b floatValue]));
    
    resolve(result);
}

+ (BOOL)requiresMainQueueSetup {
    return YES;
}



- (void)setBridge:(RCTBridge *)bridge {
    _bridge = bridge;
    _setBridgeOnMainQueue = RCTIsMainQueue();
    [self installLibrary];
}

-(void)installLibrary {
    RCTCxxBridge *cxxBridge = (RCTCxxBridge *)self.bridge;
    if (!cxxBridge.runtime) {
        
        /**
         * This is a workaround to install library
         * as soon as runtime becomes available and is
         * not recommended. If you see random crashes in iOS
         * global.xxx not found etc. use this.
         */
        
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.001 * NSEC_PER_SEC),
                       dispatch_get_main_queue(), ^{
            /**
             When refreshing the app while debugging, the setBridge
             method is called too soon. The runtime is not ready yet
             quite often. We need to install library as soon as runtime
             becomes available.
             */
            [self installLibrary];
            
        });
        return;
    }
    
    example::install(*(facebook::jsi::Runtime *)cxxBridge.runtime);
}
//RCT_EXPORT_METHOD(createServer:(NSDictionary *)args
//                  withResolver:(RCTPromiseResolveBlock)resolve
//                  withReject:(RCTPromiseRejectBlock)reject)
//{
//    example::install;
//    NSNumber *result = @(example::multiply([a floatValue], [b floatValue]));
//
//    resolve(result);
//}

@end
