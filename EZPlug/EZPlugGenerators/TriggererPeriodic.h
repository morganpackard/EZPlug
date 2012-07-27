//
//  TriggererPeriodic.h
//  Thicket II
//
//  Created by morganpackard on 12/10/11.
//  Copyright (c) 2011 Interval Studios. All rights reserved.
//

/*
    Accepts any object with a "trigger" method as a source
*/

#ifndef Thicket_II_TriggererPeriodic_h
#define Thicket_II_TriggererPeriodic_h


#include "Stk.h"
using namespace stk;
#include "Generator.h"
#include "Mixer.h"
#include "EZPlugTriggerable.h"
#include "FixedValue.h"
#include <assert.h>

class TriggererPeriodic : public Generator{

private:
    EZPlugTriggerable* triggerable;
    Generator* triggerFrequency;
    int sampleCount;
    StkFrames triggerFreqFrames;
protected:
    
public:
    
    TriggererPeriodic(){
        triggerFreqFrames.resize(kSynthesisBlockSize, 1);
        sampleCount = 0;
    }
    
    ~TriggererPeriodic(){
    }
    
    inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
        triggerFrequency->tick(triggerFreqFrames);
        float triggerFreqFloat = triggerFreqFrames(0,0);
        int samplesPerTrigger = 0;
        if(triggerFreqFloat != 0 ){
            samplesPerTrigger = Stk::sampleRate() * (1 / triggerFreqFloat);
        } 
        assert(triggerFreqFloat < 10000);
    
        triggerable->tick(frames);
        sampleCount += frames.frames();
        if(samplesPerTrigger > 0 && sampleCount >= samplesPerTrigger){
            sampleCount = sampleCount % samplesPerTrigger;
            triggerable->trigger();
        }
        return frames;
    }

    TriggererPeriodic* setTriggerable(EZPlugTriggerable* triggerableArg){
        triggerable = triggerableArg;
        return this;
    }
    
    TriggererPeriodic* setFrequency(Generator* triggerFrequencyArg){
        triggerFrequency = triggerFrequencyArg;
        return this;
    }
    
    
    
};

#endif
