//
//  Delayer.h
//  Thicket II
//
//  Created by Morgan Packard on 9/23/11.
//  Copyright 2011 Interval Studios. All rights reserved.
//

#ifndef Delayer_HEAD
#define Delayer_HEAD

#include "Stk.h"
using namespace stk;
#include "Generator.h"
#include "Mixer.h"
#import "Echo.h"

class Delayer : public Generator{
    
private: 
    
    StkFrames inputFrames;
	Echo echo1;
	Echo echo2;
    
    Generator* input;
    int numInputChannels;
    
public: 
    
    Delayer(float delayTime, int numInputChannels = 1)
    :   echo1(delayTime * Stk::sampleRate()), 
        echo2(delayTime * 0.9 * Stk::sampleRate()),
        numInputChannels(numInputChannels)
    {
        inputFrames.resize(kSynthesisBlockSize, numInputChannels);
        
    }
    
    Delayer* setInput(Generator* inputArg){
        input = inputArg;
        return this;
    }
	
	inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
        input->tick(inputFrames);
		echo1.tick(inputFrames, frames, 0, 0);
        echo2.tick(inputFrames, frames, numInputChannels - 1, 1);
        
		return frames;
	}
    
    Delayer* setEffectMix(float mix){
        echo1.setEffectMix(mix);
        echo2.setEffectMix(mix);
        return this;
    }
    
};

#endif


