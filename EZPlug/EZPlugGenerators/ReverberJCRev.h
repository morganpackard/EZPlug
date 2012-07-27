//
//  ReverberPRCRev.h
//  Thicket
//
//  Created by Morgan Packard on 7/12/11.
//  Copyright 2011 Interval Studios. All rights reserved.
//

/*
    Mono input, stereo output.
*/

#ifndef ReverberJCRev_HEAD
#define ReverberJCRev_HEAD

#include "Stk.h"
using namespace stk;
#include "Generator.h"
#include "Mixer.h"
#include "JCRev.h"
#include "AudioConfiguration.h"

class ReverberJCRev : public Generator{

private:
	Mixer mixer;
    Generator* audioSource; 
    JCRev verb;
    StkFrames reverbInput;
    StkFrames reverbOutput;
    float mix;
    
protected:
    
public:
    
    ReverberJCRev(){
        verb.setEffectMix(1.0);
        verb.setT60(1.0);
        reverbInput.resize(kSynthesisBlockSize, 1);
        reverbOutput.resize(kSynthesisBlockSize, 2);
        mix = 0.1;
    }
    
    ~ReverberJCRev(){
    }
    
    inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
        audioSource->tick(frames);
        for (int i = 0; i < kSynthesisBlockSize; i++) {
            reverbInput(i,0) = (frames(i, 0) + frames(i, 1)) / 2;
        }
        
        verb.tick(reverbInput, reverbOutput);
        
        for (int i = 0; i < kSynthesisBlockSize; i++) {
            reverbOutput(i,0) *= mix;
            reverbOutput(i,1) *= mix;
        }
        
        frames += reverbOutput;
        
        return frames;
    }
    
    ReverberJCRev* setAudioSource(Generator* source){
        audioSource = source;
        return this;
    }

    ReverberJCRev* setMix(float mixArg){
        mix = mixArg;
        return this;
    }
    
    ReverberJCRev*  setT60(float t60){
        verb.setT60(t60);
        return this;
    }
    
};

#endif

