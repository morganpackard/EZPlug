//
//  EZPlgViewController.m
//  EZPlugDemo
//
//  Created by Morgan Packard on 12/29/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "EZPlgViewController.h"


extern std::string kCppBaseAudioFilePath;

@implementation EZPlgViewController

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc that aren't in use.
}


// This method converts samples from the format EZPlug uses to the format the RemoteIO audiounit uses, and fills the buffer supplied by the
// RemoteIO callback. 
-(void) fillBuffer:(UInt32 *)frameBuffer numFrames:(UInt32)inNumberFrames{
    
    static const int blockSizeTimes2 = kSynthesisBlockSize * 2;
	SInt16 *frameBuffer16 = (SInt16*) frameBuffer;
    
    for(int i = 0; i<inNumberFrames * 2; i++){
        if(synthBufferReadPosition == 0){
            synth->tick(synthFrames);
        }
        *frameBuffer16++ = synthFrames[synthBufferReadPosition++]  * INT16_MAX;
        if(synthBufferReadPosition == blockSizeTimes2){
            synthBufferReadPosition = 0;
        }
       
    }
}



-(IBAction)volumeSliderMoved:(id)sender{
    // warning. This is not thread safe!
    synth->setFixedValue("simpleSineVolume", synth1Volume.value * 0.2);
}

-(IBAction)triggerSineSynth:(id)sender{
    synth->trigger("fmSineSynthEnvelope");
}

-(IBAction)sineSynthPitchSliderMoved:(id)sender{
    synth->setFixedValue("fmSineSynthFreq", sineSynthPitch.value);
}

-(IBAction)sineSynthFMSliderMoved:(id)sender{
    synth->setFixedValue("fmSineSynthFMAmount", sineSynthFM.value);
}


-(IBAction)setFilePlayVolume:(id)sender{
    
    UISlider * slider = (UISlider *)sender;
    
    synth->setFixedValue("filePlayVol", slider.value);
}

-(IBAction)setTremoloFrequencyForAudioFile:(id)sender{
    
    UISlider * slider = (UISlider *)sender;
    
    synth->setFixedValue("tremeloFreq", slider.value);

}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    // todo explain what this is
    
    synth = new EZPlugDemoSynth([[[NSBundle mainBundle] bundlePath] UTF8String]);

    synthFrames.resize(2, kSynthesisBlockSize);
    rioPlayer = [[RemoteIOPlayer alloc]init];
	[rioPlayer intialiseAudio];
    rioPlayer.source = self;
    synthBufferReadPosition = 0;
    [rioPlayer start];
    
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}

-(void)dealloc{
    delete synth;
}

@end
