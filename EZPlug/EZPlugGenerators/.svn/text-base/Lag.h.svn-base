#ifndef Lag_HEAD
#define Lag_HEAD

/*
 *  Lag.h
 *  BallOfString
 *
 *  Created by Morgan Packard on 1/13/11.
 *  Copyright 2011. All rights reserved.
 *
 */

#include "Stk.h"
#include "EZPlugGenerator.h"
#include "FixedValue.h"

using namespace stk;

class Lag : public EZPlugGenerator{

private:
	StkFloat target;
    FixedValue* targetFV;
    float lastFixedValueTarget;
    bool usesFVForTarget;
	StkFloat current;
	bool targetSet;
	int samplesOfLag;
    FixedValue* samplesOfLagFV;
    bool usesFVForSamplesOfLag;
    float lastFVSamplesOfLag;
	StkFloat incrementAmount;
public:
	
	Lag(){
		targetSet = false;
		samplesOfLag = 1000;
        current = 0;
        target = 0;
        usesFVForTarget = false;
        usesFVForSamplesOfLag = false;
	}
	
	Lag*  setSamplesOfLag(int numSamples){
		samplesOfLag = std::max(numSamples, 1);
        return this;
	}
	
	Lag*  setSamplesOfLag(FixedValue* value){
		
        samplesOfLagFV = value;
		usesFVForSamplesOfLag = true;
        lastFVSamplesOfLag = samplesOfLagFV->value;
        setSamplesOfLag(samplesOfLagFV->value);
        
        return this;
	}
    
    
	Lag*  setSecondsOfLag(float seconds){
		setSamplesOfLag(Stk::sampleRate() * seconds);
        return this;
	}
	
	Lag* setTarget(StkFloat val){
		target = val;
		if (!targetSet) {
			current = target;
			targetSet = true;
		}
		incrementAmount = (target - current) / (float)samplesOfLag;
		
		//printf("Lag.h setTarget called. current is: %f. incrementAmount is: %f. target is: %f\n", current, incrementAmount, target);
        return this;
	}
    
    Lag* setTarget(FixedValue* val){
		targetFV = val;
		usesFVForTarget = true;
        lastFixedValueTarget = targetFV->value;
        setTarget(targetFV->value);
		return this;
	}
    
    float getTarget(){
        return target;
    }
	
	inline StkFloat tick(){
        
        
        current += incrementAmount;
        
        if(incrementAmount > 0){
            if((target - current) < 0){
                current = target;
            }
        }else{
            if((current - target) < 0){
                current = target;
            }
        }
    
		//printf("Lag.h tick called. current is: %f. incrementAmount is: %f. target is: %f\n", current, incrementAmount, target);
		return current;
	}
    
    StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
    
        if(usesFVForSamplesOfLag && lastFVSamplesOfLag != samplesOfLagFV->value){
            lastFVSamplesOfLag = samplesOfLagFV->value;
            setSamplesOfLag(samplesOfLagFV->value);
        }
    
        if(usesFVForTarget && lastFixedValueTarget != targetFV->value){
            lastFixedValueTarget = targetFV->value;
            setTarget(targetFV->value);
        }
        
        StkFloat *samples = &frames[channel];
        for ( unsigned int i=0; i<frames.frames(); i++ ) {
            
            current += incrementAmount;
            
            if(incrementAmount > 0){
                if((target - current) < 0){
                    current = target;
                }
            }else{
                if((current - target) < 0){
                    current = target;
                }
            }
            
            *samples++ = current; 
            
        }
        
        return frames;
    }
	
};

typedef  Lag Lagger; // using the "er" convention

#endif