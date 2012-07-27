//
//  Sequencer.h
//  Thicket
//
//  Created by Morgan Packard on 5/31/11.
//  Copyright 2011 Interval Studios. All rights reserved.
//

// Step sequencer. Assumes that all events will happen within a quantized grid of relatively low resolution. 

#ifndef Sequencer_HEAD
#define Sequencer_HEAD

#include <cassert>
#include "stdio.h"
#include <algorithm>
#include "AudioConfiguration.h"
#include <vector>
// apple only
#import <dispatch/dispatch.h>
using namespace std;

class EZPlugSynth;

struct SequencerStepCallback {
  void (*fn)(int stepIndex, void* context);
  void* context;
};

class AbstractSequencer {
 public:
 virtual void step() = 0;
 virtual void advanceBySamples(int samples) = 0;
 virtual void setSynth(EZPlugSynth* synthArg) = 0;
};

//default callback that does nothing
static void emptyStepCallbackFunction(int stepIndex, void* context){
  // do nothing
}

template<class StepType, class SynthType, int maxLength=16> class Sequencer : public AbstractSequencer{

private:
    
protected:
	int nextStep;
	SynthType *synth;
	StepType *steps[maxLength]; // this should just be an array, not an array of pointers, but had trouble changing it
	StepType *currentStep;
  int length;
  float bpm;
  float samplesPerStep;
  float sampleCount;
  float stepsPerBeat;
  StepType defaultStep;
  std::vector<SequencerStepCallback> stepCallbacks;
  
    
  void init(){
    bpm = -1;
    stepsPerBeat = 4;
    sampleCount = -1;
    length = maxLength;
    nextStep = 0;
    for (int i =0; i < maxLength; i++) {
      steps[i] = new StepType();
    }
    
  }
    
public: 
	
	Sequencer(SynthType* synthArg){
        init();
        synth = synthArg;
	}
    
    Sequencer(){
        init();
    }
	
	~Sequencer(){
        for (int i = 0; i < maxLength; i++) {
            delete steps[i];
        }
	}
    
  void reset(){
    StepType* freshStep = new StepType();
    
    for (int i =0; i < maxLength; i++) {
      *steps[i] = *freshStep;
    }
    
    delete freshStep;
    
    nextStep = 0;
      
  }
    
  void resetWithoutChangingPosition(){
      
    for (int i =0; i < maxLength; i++) {
      *steps[i] = defaultStep;
    }
      
  }
  
  void setSynth(EZPlugSynth* synthArg){
      synth = static_cast<SynthType*>(synthArg);
  }
  
  SynthType* getSynth(){
    return synth;
  }
  
  int getLength(){
    return length;
  }
	
	void setsynth(SynthType *synthArg){
		synth = synthArg;
	}
	
	virtual void step(){
    
    //save this variable, because the async call happens _after_ nextStep has been incremented
    int nextStepForBlock = nextStep;
    dispatch_async(dispatch_get_main_queue(), ^{
      for(int i = 0; i < stepCallbacks.size(); i++){
          SequencerStepCallback cb = stepCallbacks.at(i);
          cb.fn(nextStepForBlock, cb.context);
      }
    });
    
    currentStep = steps[nextStep];
    
    currentStep->set(synth);
    
    nextStep++;
    
    if (nextStep >= length) {
        nextStep = 0;
    }
	}
	
	int getNextStepIndex(){
		return nextStep;
	}
	
	int getCurrentStepIndex(){
		int index = nextStep - 1;
		if (index < 0) {
			index = length - 1;
		}
		return index;
	}
    
    
  StepType* getNextStep(){
      return steps[getNextStepIndex()];
  }
  
  void setLength(int lengthArg){
      
    length = min(maxLength, lengthArg);
    
    if(nextStep >= lengthArg ){
        nextStep = 0;
    }
  }
  
  StepType* getStep(const int index){
    if(index >= length){
      printf("Error in Sequencer. Tried to access a step that doesn't exist. This Sequencer's length is: %i, requested step index was: %i.", getLength(), index);
      exit(0);
    }
    return steps[index];
  }
  
  StepType* operator[](const int index){
      return steps[index];
  }
  
  void setStep(int step){
      nextStep = step;
  }
  
  void growLengthByOne(){
      setLength(length + 1);
  }
  
  void setbpm(float bpmArg){
    bpm = bpmArg;
    const float secondsPerMinute = 60;
    float beatsPerSample = bpm / (secondsPerMinute * kSampleRate);
    samplesPerStep = 1 / (beatsPerSample * stepsPerBeat);
    // set things up so we get a tick on the first sample grab
    if(sampleCount == -1){
      sampleCount = samplesPerStep;
    }
  }
  
  void advanceBySamples(int samples){
    if(bpm == -1){
      printf("ERROR. You must set the bpm of a sampler before calling advanceBySamples\n");
    }
    sampleCount += (float)samples;
    if(sampleCount >= samplesPerStep){
        step();
        sampleCount -= samplesPerStep;
    }
  }
  
  void addCallback(SequencerStepCallback callback){
    stepCallbacks.push_back(callback);
  }
    
};

#endif

