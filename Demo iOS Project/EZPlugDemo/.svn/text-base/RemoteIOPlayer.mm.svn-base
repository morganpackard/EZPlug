
#import "RemoteIOPlayer.h"
#include <AudioUnit/AudioUnit.h>\

@implementation RemoteIOPlayer

@synthesize source;

AudioComponentInstance audioUnit;
AudioStreamBasicDescription audioFormat;

#define kSampleRate 44100
#define kBuffersize 1024
#define kOutputBus 0
#define kInputBus 1

void rioInterruptionListener(void *inClientData, UInt32 inInterruption)
{
	printf("Session interrupted! --- %s ---", inInterruption == kAudioSessionBeginInterruption ? "Begin Interruption" : "End Interruption");
	
	RemoteIOPlayer *rioPlayer = (RemoteIOPlayer*)inClientData;
	
	if (inInterruption == kAudioSessionEndInterruption) {
		[rioPlayer start];
	}
	
	if (inInterruption == kAudioSessionBeginInterruption) {
		[rioPlayer stop];
    }
}

-(id) init{
    [super init];
    return self;
}

-(OSStatus)start{
//	NSLog(@"start called");
	OSStatus status = AudioOutputUnitStart(audioUnit);
	
    BOOL mixWithOthers = TRUE;
	AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryMixWithOthers, sizeof(mixWithOthers), &mixWithOthers);
    
	return status;
}

-(OSStatus)stop{
	OSStatus status = AudioOutputUnitStop(audioUnit);
	return status;
}

-(void)cleanUp{
	AudioUnitUninitialize(audioUnit);
}


static OSStatus playbackCallback(void *inRefCon, 
								 AudioUnitRenderActionFlags *ioActionFlags, 
								 const AudioTimeStamp *inTimeStamp, 
								 UInt32 inBusNumber, 
								 UInt32 inNumberFrames, 
								 AudioBufferList *ioData) {  
	
	RemoteIOPlayer *remoteIOplayer = (RemoteIOPlayer *)inRefCon;
	
	for (int i = 0 ; i < ioData->mNumberBuffers; i++){
		AudioBuffer buffer = ioData->mBuffers[i];
		UInt32 *frameBuffer = (UInt32 *)buffer.mData;
		[[remoteIOplayer source] fillBuffer:frameBuffer numFrames:inNumberFrames];	    

	}
    

    
	return noErr;
}

-(void)intialiseAudio{
	
	sampleRate = kSampleRate;
	
	// initialise the audio session
	AudioSessionInitialize(NULL, NULL, rioInterruptionListener, self);
    
    
    Float64 readSampleRate;
    UInt32 propertySize = sizeof (sampleRate);    // 1
 
    AudioSessionGetProperty (                                  // 2
        kAudioSessionProperty_CurrentHardwareSampleRate,
        &propertySize,
        &readSampleRate
    );
    
	
	UInt32 audioCategory = kAudioSessionCategory_AmbientSound;   
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(audioCategory), &audioCategory);
    
    
//    UInt32 mixWithOthers = TRUE;
//	AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryMixWithOthers, sizeof(mixWithOthers), &mixWithOthers);
    
	
	// set the sample rate of the session
	AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareSampleRate, sizeof(sampleRate), &sampleRate);
//	NSLog(@"AudioSession === setting PreferredHardwareSampleRate to %.0fHz.", sampleRate);
	
	// set buffer size
	Float32 bufferSize = kBuffersize; // requested buffer size
	Float32 bufferDuration = bufferSize / kSampleRate; // buffer duration in seconds
	AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration, 
							sizeof(bufferDuration), &bufferDuration);
	
	AudioSessionSetActive(true);
	
	// print value of properties to check that everything was set properly
	Float64 audioSessionProperty64 = 0;
	Float32 audioSessionProperty32 = 0;
	UInt32 audioSessionPropertySize64 = sizeof(audioSessionProperty64);
	UInt32 audioSessionPropertySize32 = sizeof(audioSessionProperty32);
	AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareSampleRate, 
							&audioSessionPropertySize64, &audioSessionProperty64);
//	NSLog(@"AudioSession === CurrentHardwareSampleRate: %.0fHz", audioSessionProperty64);
	sampleRate = audioSessionProperty64;
	
	AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareIOBufferDuration, 
							&audioSessionPropertySize32, &audioSessionProperty32);
	int blockSize = lrint(audioSessionProperty32 * audioSessionProperty64);
//	NSLog(@"AudioSession === CurrentHardwareIOBufferDuration: %3.2fms", audioSessionProperty32*1000.0f);
//	NSLog(@"AudioSession === block size: %i", blockSize);
	
	
	
	
	
	// Describe audio component 
	AudioComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_RemoteIO;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	
	// Get component
	AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);
	
	OSStatus status;
	
	// Get audio units
	status = AudioComponentInstanceNew(inputComponent, &audioUnit);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////        Enable IO section         //////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*
		Audio output is working. 
	 */
	
	UInt32 affirmative = 1;
	// Enable IO for playback
	status = AudioUnitSetProperty(audioUnit, 
								  kAudioOutputUnitProperty_EnableIO, 
								  kAudioUnitScope_Output, 
								  kOutputBus,
								  &affirmative, 
								  sizeof(affirmative));
	
	
	/*
	 Audio input isn't working. Am I doing something wrong here, or somewhere else? 
	 */
	
	// connect the AU to the microphone 
//	status = AudioUnitSetProperty(audioUnit, 
//						 kAudioOutputUnitProperty_EnableIO, 
//						 kAudioUnitScope_Input, 
//						 kInputBus, 
//						 &affirmative, 
//						 sizeof(affirmative));
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	// Describe format
	audioFormat.mSampleRate			= (float)kSampleRate;
	audioFormat.mFormatID			= kAudioFormatLinearPCM;
	audioFormat.mFormatFlags		= kAudioFormatFlagsCanonical ;
	audioFormat.mFramesPerPacket	= 1;
	audioFormat.mChannelsPerFrame	= 2;
	audioFormat.mBitsPerChannel		= 16;
	audioFormat.mBytesPerPacket		= 4;
	audioFormat.mBytesPerFrame		= 4;
	
	//Apply format
	status = AudioUnitSetProperty(audioUnit, 
								  kAudioUnitProperty_StreamFormat, 
								  kAudioUnitScope_Input, 
								  kOutputBus, 
								  &audioFormat, 
								  sizeof(audioFormat));
	
	
	status = AudioUnitSetProperty(audioUnit, 
								  kAudioUnitProperty_StreamFormat, 
								  kAudioUnitScope_Output, 
								  kInputBus, 
								  &audioFormat, 
								  sizeof(audioFormat));
	
	 
	// Set up the playback  callback
	AURenderCallbackStruct callbackStruct;
	callbackStruct.inputProc = playbackCallback;
	//set the reference to "self" this becomes *inRefCon in the playback callback
	callbackStruct.inputProcRefCon = self;
	
	status = AudioUnitSetProperty(audioUnit, 
								  kAudioUnitProperty_SetRenderCallback, 
								  kAudioUnitScope_Input, 
								  kOutputBus,
								  &callbackStruct, 
								  sizeof(callbackStruct));
	
	// Initialise
	status = AudioUnitInitialize(audioUnit);
}



@end
