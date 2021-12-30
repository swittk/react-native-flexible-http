#import <React/RCTBridgeModule.h>

#ifdef __cplusplus

#import "react-native-flexible-http.h"

#endif
@interface FlexibleHttp : NSObject <RCTBridgeModule>
@property (nonatomic, assign) BOOL setBridgeOnMainQueue;
@end
