/*
 *  Volume.h
 *  TridentToy
 *
 *  Created by Morgan Packard on 3/4/11.
 *  Copyright 2011 intervalstudios. All rights reserved.
 *
 */

/* Simply multiples a buffer */

#ifndef Volume_HEAD
#define Volume_HEAD

#include "EZPlugGenerator.h"
#include "AudioConfiguration.h"
#include "FixedValue.h"

class Volume : public EZPlugGenerator{

private:
	StkFloat volLevel;
	Generator *audioSource;
    FixedValue* volFixedVal;
protected:


public: 
	
	Volume(StkFloat volLevelArg = 1){
		volLevel = volLevelArg;
        volFixedVal = 0;
	}
	
	~Volume(){

	}
	
	inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
    
        if(volFixedVal != 0){
            volLevel = volFixedVal->value;
        }
		
		audioSource->tick(frames, channel);
		
		for (int frame = 0; frame < frames.frames(); frame++) {
			for (int channel = 0; channel < frames.channels(); channel++) {
				frames(frame, channel) *= volLevel;
			}
		}
		return frames;
	}
	
	Volume* setAudioSource(EZPlugGenerator *audioSourceArg){
		audioSource = audioSourceArg;
        return this;
	}
	
	Volume* setVolume(StkFloat volArg){
		volLevel = volArg;
        return this;
	}
	
	Volume* setVolume(FixedValue* volArg){
		volFixedVal = volArg;
        return this;
	}
    
    StkFloat getVolume(){
        return volLevel;
    }
	
};

#endif



