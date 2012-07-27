
#ifndef Mixer_head
#define Mixer_head

/*
 *  untitled.h
 *  BallOfString
 *
 *  Created by Morgan Packard on 1/12/11.
 *  Copyright 2011. All rights reserved.
 *
 */

#include "EZPlugProcessor.h"

class Mixer : public EZPlugProcessor {
protected:
	Generator *audioSources[200];
	int numVoicesAdded;
	StkFrames *workSpace;
	StkFrames *lastFrames; // in case we want to grab these and examine them
public:
	Mixer(int numChannels = 2){
		numVoicesAdded = 0;
		workSpace = new StkFrames(kSynthesisBlockSize, numChannels);
	}
	
	~Mixer(){
		delete workSpace;
	}
	
	// All generators must have the same number of channels, and this must match the number of channels 
	// in frames passed to tick.
    // deprecated. Use addInput instead
	Mixer* addGenerator(Generator *generator){
		audioSources[numVoicesAdded] = generator;
		numVoicesAdded++;
        return this;
	}
    
    Mixer* addInput(Generator* obj){
        return addGenerator(obj);
    }
	
	inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
		
		StkFloat *framesData =  &frames[0];
		
		memset(framesData, 0, sizeof(StkFloat) * frames.size());
		
		for (int j =0; j < numVoicesAdded; j++) {
        audioSources[j]->tick(*workSpace, 0);
           
        #ifdef USE_APPLE_ACCELERATE
            vDSP_vadd(&(*workSpace)[0], 1, &frames[0], 1, &frames[0], 1, frames.frames() * frames.channels());
        #else
            frames += *workSpace; // add each sample in frames to each sample in workspace
        #endif
               
		}
		
		lastFrames = &frames;
		
		return frames;
	}
	
	StkFrames* getLastFrames(){
		return lastFrames;
	}
    
    StkFrames* mixLastFramesDownToMono(StkFrames* monoFrames){
        int numChannels = lastFrames->channels();
        float numChannelsFloat = (float)numChannels;
        
        for(int i = 0; i < lastFrames->frames(); i++){
            (*monoFrames)(i, 0) = 0;
            for (int j = 0; j < numChannels; j++) {
                (*monoFrames)(i, 0) += (*lastFrames)(i,j);
            }
            (*monoFrames)(i, 0) /= numChannelsFloat;
        }
        return monoFrames;
    }
	
	
};


class Adder : public Mixer {

public:
  // default to only one channel;
  Adder(int numChannels = 1): Mixer(numChannels) {};
};

#endif