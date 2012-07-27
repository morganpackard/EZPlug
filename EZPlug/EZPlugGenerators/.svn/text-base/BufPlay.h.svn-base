/*
 *  BufPlay.h
 *  Thicket
 *
 *  Created by Morgan Packard on 3/1/11.
 *  Copyright 2011. All rights reserved.
 *
 */

#ifndef BufPlay_HEAD
#define BufPlay_HEAD

#include "Stk.h"
#import "AudioConfiguration.h"
#import "FileRead.h"
#include <algorithm>
#include "EZPlugTriggerable.h"
#include "FixedValue.h"

class BufPlay : public EZPlugTriggerable{
    
private:
	StkFrames *sample;
	int sampleReadPosition;
	int sampleStartPosition;
	int sampleEndPosition;
    bool doesOwnFrames; // indicate whether an StkFrames was passed in via the constructor, or whether we built it ourselves
                        // if we built it ourselves, we need to destroy it too.
    bool isFinished;
    
    bool loop; // not used any more. would remove, but there's some weird memory stuff going on
    FixedValue* startPositionFV;
    bool useFVForPosition;
protected:

    void initWithSample(StkFrames *sampleArg){
        
		sample = sampleArg;
		sampleStartPosition = 0;
		sampleEndPosition = sample->frames();
		sampleReadPosition = sampleEndPosition;
        doesOwnFrames = false;
        isFinished = false;
        loop = false;
        startPositionFV = 0;
        useFVForPosition = false;
    }
    
    
public: 
	
    
	BufPlay(StkFrames *sampleArg){
        initWithSample(sampleArg);
	}
	
	BufPlay(std::string path){
        doesOwnFrames = true;
        
        FileRead* read = new FileRead( path );
        sample = new StkFrames(read->fileSize(), 2);
        read->read(*sample);
        delete read;

        initWithSample(sample);        
	}
	
	~BufPlay(){
        if(doesOwnFrames){
            delete sample;
        }
	}
	
	inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
		
		if(sampleReadPosition <= sampleEndPosition - kSynthesisBlockSize){
			for (int i = 0; i < kSynthesisBlockSize; i++) {
				for (int j = 0; j < frames.channels(); j++) {
					frames(i,j) = (*sample)(sampleReadPosition, j);
				}
				sampleReadPosition++;
			}
		}else {
            isFinished = true;
			for (int i = 0; i < kSynthesisBlockSize; i++) {
				for (int j = 0; j < frames.channels(); j++) {
					frames(i,j) = 0;
				}
			}
		}
        
        if(loop && isFinished){
            trigger();
        }
		
		return frames;
	}
	
	void trigger(){
        isFinished = false;
        if(useFVForPosition){
            setStartPointPercent(startPositionFV->value);
        }
		sampleReadPosition = sampleStartPosition;
	}
  
  BufPlay* start(){
    trigger();
    return this;
  }
	
	void setDurationSeconds(StkFloat seconds){
		sampleEndPosition = seconds * kSampleRate;
	}
    
    bool hasData(){
        return !isFinished;
    }
    
    void setStartPointPercent(float percent){
        if(percent < 0){
            percent = 0;
        }
        sampleStartPosition = percent * sampleEndPosition;
    }
    
    void setStartTime(float seconds){
        sampleStartPosition = Stk::sampleRate() * seconds;
    }
    
    
    BufPlay* setStartPointPercent(FixedValue* percent){
        useFVForPosition = true;
        startPositionFV = percent;
        return this;
    }
    
    BufPlay*  setDoesLoop(bool loopArg){
        loop = loopArg;
        return this;
    }
	
};

#endif

