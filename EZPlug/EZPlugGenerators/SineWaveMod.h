//
//  SineWaveMod.h
//  Thicket II
//
//  Created by Morgan Packard on 9/16/11.
//  Copyright 2011 Interval Studios. All rights reserved.
//

#ifndef SineWaveMod_H
#define SineWaveMod_H

#include "SineWave.h"
#include "AudioConfiguration.h"

namespace stk {

class SineWaveMod : public SineWave{
    
protected:
    
    Generator* frequencyGenerator;
    StkFrames modFrames;
    StkFrames *pointerToTable;
    
public:
    
    SineWaveMod(){
        modFrames.resize(kSynthesisBlockSize, 1);
        pointerToTable = &table_;
    }
    
    virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    
    SineWaveMod* setFrequency(Generator* genArg){
        frequencyGenerator = genArg;
        return this;
    }  
}; 


inline StkFrames& SineWaveMod :: tick( StkFrames& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
    if ( channel >= frames.channels() ) {
        errorString_ << "SineWave::tick(): channel and StkFrames arguments are incompatible!";
        handleError( StkError::FUNCTION_ARGUMENT );
    }
#endif
    
    frequencyGenerator->tick(modFrames);
    
    StkFloat *samples = &frames[channel];
    StkFloat tmp = 0.0;
    
    StkFloat *freqBuffer = &modFrames[0];
    
    unsigned int hop = frames.channels();
    for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
        
        SineWave::setFrequency(*(freqBuffer++));
        
        // Check limits of time address ... if necessary, recalculate modulo
        // TABLE_SIZE.
        while ( time_ < 0.0 )
            time_ += TABLE_SIZE;
        while ( time_ >= TABLE_SIZE )
            time_ -= TABLE_SIZE;
        
        iIndex_ = (unsigned int) time_;
        alpha_ = time_ - iIndex_;
        tmp = (*pointerToTable)[ iIndex_ ];
        tmp += ( alpha_ * ( (*pointerToTable)[ iIndex_ + 1 ] - tmp ) );
        *samples = tmp;
        
        // Increment time, which can be negative.
        time_ += rate_;
    }
    
    lastFrame_[0] = tmp;
    return frames;
}
    
}

#endif