//
//  GeneratorAdder.h
//  Thicket II
//
//  Created by Morgan Packard on 9/17/11.
//  Copyright 2011 Interval Studios. All rights reserved.
//

/* Simply multiples a buffer */

#ifndef Multiplier_HEAD
#define Multiplier_HEAD


#include "EZPlugGenerator.h"
#include "string.h"
#include <vector>

// todo warn if trying to multiply buffers of disparate numbers of channels

class Multiplier : public EZPlugGenerator{
    
private:
	StkFrames workSpace;
	Generator *val1;
	Generator *val2;
    std::vector<Generator*> vals;
protected:
    
    
public: 
	
	Multiplier(Generator* val1Arg, Generator* val2Arg){
        workSpace.resize(kSynthesisBlockSize, 1);
        val1 = val1Arg;
        val2 = val2Arg;
	}
    
    Multiplier(int numChannels = 1){
        workSpace.resize(kSynthesisBlockSize, numChannels);
	}
    
    ~Multiplier(){
        // iterate over valFrames and delete
    }
	
	inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
		
        if(frames.channels() != workSpace.channels()){
          printf("ERROR. Number of channels in frames argument to Multiplier::tick does not match the number of channels used when constructing Multiplier.");
          exit(0);
        }
    
        memset(&frames[0], 0, sizeof(StkFloat) * frames.size());
        
        // for the first generator, store the value in the frame argument
        vals[0]->tick(frames);
        
        // for additional generators, use the workspace stkframes for tick, and multiply it into the frames argument
        for(std::vector<Generator*>::size_type i = 1; i < vals.size(); i++) {
            vals[i]->tick(workSpace);
            float* framesPointer = &frames[0];
            float* workspacePointer = &workSpace[0];
            
            
            #ifdef USE_APPLE_ACCELERATE
                vDSP_vmul (
                   framesPointer,       // __vDSP_input1[],
                   1,                   //__vDSP_stride1,
                   workspacePointer,    //const float __vDSP_input2[],
                   1,                   //__vDSP_stride2,
                   framesPointer,       //float __vDSP_result[],
                   1,                   //vDSP_Stride __vDSP_strideResult,
                   frames.frames() * frames.channels() //vDSP_Length __vDSP_size
                );
            #else
                for (int sampleCount = 0; sampleCount < frames.frames() * frames.channels(); sampleCount++) {
                    *(framesPointer++) *= *(workspacePointer++);
                }
            #endif
            

        }
        
		return frames;
	}
    
    Multiplier* setVals(Generator* val1Arg, Generator* val2Arg){
        val1 = val1Arg;
        val2 = val2Arg;
        addVal(val1Arg);
        addVal(val2Arg);
        return this;
    }
	
    Multiplier* setVal1(Generator* val1Arg){
        val1 = val1Arg;
        addVal(val1Arg);
        return this;
    }
    
    
    Multiplier* setVal2(Generator* val2Arg){
        val2 = val2Arg;
        addVal(val2Arg);
        return this;
    }
    
    Multiplier* addVal(Generator* valArg){
        vals.push_back(valArg);
        return this;
    }
	
};

#endif



