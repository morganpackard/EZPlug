//
//  EZClipper.h
//  Thicket II
//
//  Created by Morgan Packard on 1/18/12.
//  Copyright (c) 2012 Interval Studios. All rights reserved.
//

#ifndef Thicket_II_EZClipper_h
#define Thicket_II_EZClipper_h

#include "Stk.h"
#include "AudioConfiguration.h"

class EZClipper : public Generator {
  
private:

    Generator* input;
    Generator* clipLevel;
    StkFrames clipLevelFrames;

public:

    EZClipper(){
        clipLevelFrames.resize(kSynthesisBlockSize, 1);
    }

    EZClipper* setInput(Generator* inputArg){
        input = inputArg;
        return this;
    }
    
    EZClipper* setClipLevel(Generator* clipLevelArg){
        clipLevel = clipLevelArg;
        return this;
    }

  	inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
        
        clipLevel->tick(clipLevelFrames);
        input->tick(frames);
        
        for (int i = 0; i < frames.frames(); i++) {
            for (int j = 0; j < frames.channels(); j++) {
                frames(i,j) = std::max( 
                  std::min(frames(i,j), std::abs(clipLevelFrames(i,0))),
                  -std::abs(clipLevelFrames(i,0))
                );
            }
        }
        
		return frames;
	}
};

#endif
