//
//  FixedValue.h
//  Thicket II
//
//  Created by Morgan Packard on 9/14/11.
//  Copyright 2011 Interval Studios. All rights reserved.
//

#ifndef FixedValue_HEAD
#define FixedValue_HEAD

#include "Stk.h"
using namespace stk;
#include "Generator.h"
#include "Mixer.h"
#include <algorithm>

class FixedValue : public Generator{

private:
	Mixer *mixer;
    
protected:
    
public:
    
    float value;
    
    FixedValue(float val) : value(val){
    }
    
    FixedValue(){
    }
     
    ~FixedValue(){
    }
    
    inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
        float* buffStart = &frames[0];
        std::fill(buffStart, buffStart + frames.size(), value);
        return frames;
    }

    
};

#endif

