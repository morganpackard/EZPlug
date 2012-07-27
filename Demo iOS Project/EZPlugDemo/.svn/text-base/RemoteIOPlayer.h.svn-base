
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "EZPlgViewController.h"

@class EZPlgViewController;

@interface RemoteIOPlayer : NSObject {
	EZPlgViewController* source;
	Float64 sampleRate;
}
 
@property (nonatomic, retain) EZPlgViewController* source;

-(OSStatus)start;
-(OSStatus)stop;
-(void)cleanUp;
-(void)intialiseAudio;

@end
