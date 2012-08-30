//
//  EZPlgViewController.h
//  EZPlugDemo
//
//  Created by Morgan Packard on 12/29/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RemoteIOPlayer.h"
#import "EZPlgDemoSynth.h"
#import "AudioConfiguration.h"

@class RemoteIOPlayer;

@interface EZPlgViewController : UIViewController{
    RemoteIOPlayer* rioPlayer;
    
    // EZPlugDemoSynth is where the entire synthesis network is constructed. 
    // You may notice it's not a pointer. It could be, but, unlike in iOS, it's common
    // to have member variable objects in C++ that aren't pointers.
    EZPlugDemoSynth* synth;
    
    int synthBufferReadPosition;
    StkFrames synthFrames;
    IBOutlet UISlider* synth1Volume;
    IBOutlet UISlider* sineSynthPitch;
    IBOutlet UISlider* sineSynthFM;
}

-(IBAction)triggerSineSynth:(id)sender;
-(IBAction)volumeSliderMoved:(id)sender;
-(IBAction)sineSynthPitchSliderMoved:(id)sender;
-(IBAction)sineSynthFMSliderMoved:(id)sender;
-(IBAction)setFilePlayVolume:(id)sender;
-(IBAction)setTremoloFrequencyForAudioFile:(id)sender;

-(void) fillBuffer:(UInt32 *)frameBuffer numFrames:(UInt32)inNumberFrames;
-(void) runPerformanceTests;

@end
