#ifndef Panner_head
#define	Panner_head

/*
 *  Panner.h
 *  BallOfString
 *
 *  Created by Morgan Packard on 1/8/11.
 *  Copyright 2011. All rights reserved.
 *
 */


#include "EZPlugGenerator.h"
#import <stdio.h>
#import "Lag.h"
#import "FixedValue.h"

using namespace stk;


class Panner : public EZPlugGenerator {
private:
	Generator* source;
	StkFloat lVolume;
	StkFloat rVolume;
    StkFloat* volumes[2];
	StkFloat baseVolume;
	StkFloat volume2;
	StkFloat pan;
	Lag *volumeLag;
	StkFrames *sourceBuffer; // what we write to before panning
	FixedValue* volumeFV;
    bool usesFVForVolume;
    float lastVolumeFVValue;
    
	void inline setLRVolumes(){
		if(pan < 0){
			lVolume = 1;
			rVolume = 1 + pan;
		}else {
			rVolume = 1;
			lVolume = 1 - pan;
		}
		
		//volumeLag->setTarget(baseVolume * volume2);
		
		//StkFloat laggedVolume = volumeLag->tick();
		
		lVolume *= baseVolume * volume2;
		rVolume *= baseVolume * volume2;
        volumes[0] = &lVolume;
        volumes[1] = &rVolume;
	}
	
public:
	
	Panner(){
        lastVolumeFVValue = 0;
        usesFVForVolume = false;
		baseVolume = 1;
		volume2 = 1;
		pan = 0;
		volumeLag = new Lag();
		volumeLag->setSamplesOfLag(1);
		this->setLRVolumes();
		sourceBuffer = new StkFrames(kSynthesisBlockSize, 1);
	}
	
	~Panner(){
		delete sourceBuffer;
		delete volumeLag;
	}
	
	inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
		source->tick(*sourceBuffer, 0);
        StkFloat* sourceData = &(*sourceBuffer)[0];
        
        if(usesFVForVolume && (volumeFV->value != lastVolumeFVValue)){
            setVolume(volumeFV->value);
            lastVolumeFVValue = volumeFV->value;
        }
        
        #ifdef USE_APPLE_ACCELERATE
            for (int i = 0; i < 2; i++){
                vDSP_vsmul (
                   sourceData ,     //__vDSP_input1[],
                   1,               //__vDSP_stride1,
                   volumes[i],        //*__vDSP_input2,
                   &frames[i],      //__vDSP_result[],
                   2,               //__vDSP_strideResult,
                   frames.frames()  //__vDSP_size
                );
            }
        #else
            StkFloat *data =  &frames[0];
            for (int i = 0; i < frames.frames(); i++) {
                *data++ =  *sourceData * lVolume;
                *data++ = *sourceData * rVolume;
                sourceData++;
            }
        #endif
        
		return frames;
	}
	
	Panner* setSource(Generator *sourceArg){
		source = sourceArg;
        return this;
	}
	
	Panner*  setPan(StkFloat panArg){ // -1 to 1
		pan = panArg;
		this->setLRVolumes();
        return this;
	}
    
	Panner*  setVolume(FixedValue* vol){
        // protect against zero pointer
        if(vol != 0){
            volumeFV = vol;
            lastVolumeFVValue = vol->value;
            usesFVForVolume = true;
            setVolume(vol->value);
        }
        return this;
	}
	
	Panner*  setVolume(StkFloat vol){
		baseVolume = vol;
		this->setLRVolumes();
        return this;
	}
	
	Panner*  setVolume2(StkFloat vol){
		volume2 = vol;
		this->setLRVolumes();
        return this;
	}
	
	Panner*  setVolumeLag(int numSamples){
		volumeLag->setSamplesOfLag(numSamples);
        return this;
	}
	
	StkFloat getVolume(){
		return baseVolume;
	}

	
};

#endif