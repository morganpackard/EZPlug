//
//  ReverberPRCRev.h
//  Thicket
//
//  Created by Morgan Packard on 7/12/11.
//  Copyright 2011 Interval Studios. All rights reserved.
//

#ifndef ReverberPRCRev_HEAD
#define ReverberPRCRev_HEAD

#include "Stk.h"
using namespace stk;
#include "Generator.h"
#include "Mixer.h"
#include "PRCRev.h"
#include "AudioConfiguration.h"

class ReverberPRCRev : public Generator{

private:
	Mixer mixer;
    Generator* audioSource; 
    PRCRev verb;
    StkFrames reverbInput;
    StkFrames reverbOutput;
    float mix;
    Generator* mixGen;
    StkFrames mixGenFrames;
    
protected:
    
public:
    
    ReverberPRCRev():
        mixGen(NULL){
        verb.setEffectMix(1.0);
        verb.setT60(1.0);
        reverbInput.resize(kSynthesisBlockSize, 1);
        reverbOutput.resize(kSynthesisBlockSize, 2);
        mixGenFrames.resize(kSynthesisBlockSize, 1);
        mix = 0.1;
    }
    
    ~ReverberPRCRev(){
    }
    
    inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
    
        #ifdef _STK_DEBUG_
            if(frames.channels() !=2 ){
                Stk::handleError( "Input to ReverberPRCRev must be stereo.", StkError::MEMORY_ALLOCATION );
            }
        #endif
    
        audioSource->tick(frames);
        
        StkFloat* inputBuff = &reverbInput[0];
        StkFloat* generatorInput = &frames[0];
        StkFloat* outputBuff = &reverbOutput[0];
        
        for (int i = 0; i < kSynthesisBlockSize; i++) {
            *(inputBuff++) = (*(generatorInput++) + *(generatorInput++)) / 2;
        }
        
        verb.tick(reverbInput, reverbOutput);
        
        if(mixGen != NULL){
            mixGen->tick(mixGenFrames);
            StkFloat *mixPointer = &mixGenFrames[0];
            for (int i = 0; i < kSynthesisBlockSize; i++) {
                *(outputBuff++) *= *mixPointer;
                *(outputBuff++) *= *(mixPointer++);
            }
        }else{
            for (int i = 0; i < kSynthesisBlockSize; i++) {
                *(outputBuff++) *= mix;
                *(outputBuff++) *= mix;
            }
        }
        
        
        
        frames += reverbOutput;
        
        return frames;
    }
    
    ReverberPRCRev* setAudioSource(Generator* source){
        audioSource = source;
        return this;
    }

    ReverberPRCRev* setMix(float mixArg){
        mix = mixArg;
        return this;
    }
    
    ReverberPRCRev* setMix(Generator* mixArg){
        mixGen = mixArg;
        return this;
    }
    
    ReverberPRCRev* setT60(float t60){
        verb.setT60(t60);
        return this;
    }
    
};

#endif

