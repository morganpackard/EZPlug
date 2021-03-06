//
//  EZPlugSynth.h
//  Thicket II
//
//  Created by morganpackard on 11/14/11.
//  Copyright (c) 2011 Interval Studios. All rights reserved.
//

#ifndef Thicket_II_EZPlugSynth_h
#define Thicket_II_EZPlugSynth_h

#include "AudioConfiguration.h"

#include "Stk.h"
using namespace stk;
#include "Generator.h"
#include "Mixer.h"
#include "Panner.h"
#include "SineWaveMod.h"
#include "FixedValue.h"
#include "Enveloper.h"
#include "TriggererPeriodic.h"
#include "Multiplier.h"
#include "EZPlugHelper.h"
#include "ReverberPRCRev.h"
#include "EZClipper.h"
#include "EZPlugTriggerable.h"
#include "CPPAudioUtil.h"
#include "EZCircularBuffer.h"
#include "ORDLockFreeQueue.h"
#include "Sequencer.h"
#include <iostream>
#include <vector>
#include <map>


using namespace EZPlug;

typedef enum{
  TRIGGER,
  SET_FIXED_VALUE
}EZPlugSynthMessageType;

struct EZPlugSynthMessage {
    EZPlugSynthMessageType messageType;
    char target[40];
    float value;
};

class EZPlugSynth : public Generator {

private:
    std::vector<AbstractSequencer*>sequencers;
    std::vector<void*> generators;
    std::map<std::string, FixedValue*> fixedValues;
    std::map<std::string, EZPlugTriggerable*> triggerables;
    ORDLockFreeQueue<EZPlugSynthMessage> messageQueue;
    StkFrames outputFrames;
    // used to correllate between internal buffer and an external buffer of abritrary lenght that we need to write to
    int synthBufferReadPosition; 
    
protected:
    Generator* output;
    
    void pSetFixedValue(std::string key, float value){
        if(fixedValues[key] != NULL){
          fixedValues[key]->value = value;
        }else{
          printf("EZPlugSynth. Warning: key not found for setFixeValue. %s\n", key.c_str());
        }
    }
    
    void pTrigger(std::string key){
        if(triggerables[key] != NULL){
          triggerables[key]->trigger();
        }else{
          printf("EZPlugSynth. Warning: key not found for trigger. %s", key.c_str());
        }
    }
    
public:
    
    EZPlugSynth():synthBufferReadPosition(0){
      outputFrames.resize(kSynthesisBlockSize, 2);
    }
    
    ~EZPlugSynth(){
        // delete all the thingies
        for(std::vector<void*>::size_type i = 0; i != generators.size(); i++) {
            delete (Generator*)generators[i];
        }
        
    }
    
    void addSequencer(AbstractSequencer* seq){
      seq->setSynth(this);
      sequencers.push_back(seq);
    }
    
    // store and return a pointer to an object
    // this object will be destroyed eventually
    template<typename T> T* a(T* obj){
        generators.push_back(obj);
        return obj;
    }
    
    // store and return a pointer to a FixedValue object   
    FixedValue* v(FixedValue* obj, std::string name){
        a(obj);
        fixedValues[name] = obj;
        return obj;
    }
    
    // fetch a FixedValue object   
    FixedValue* v(std::string name){
        FixedValue* ret = fixedValues[name];
        assert(ret != NULL);
        return ret;
    }
    
    // create a fixedvalue object which will never change   
    FixedValue* v(float value){
        FixedValue* ret = a(new FixedValue());
        ret->value = value;
        return ret;
    }
    
    
    // create a fixedvalue object with a given value, store it with a given name   
    FixedValue* v(float value, std::string name){
        return v(v(value), name);
    }
    
    
    // store and return a pointer to a Triggerable object   
    template<typename T> T* t(T* obj, std::string name){
        a(obj);
        triggerables[name] = obj;
        return obj;
    }
    
    // store and return a pointer to a Triggerable object   
    template<typename T> T* t(std::string name, T* obj){
        return t(obj, name);
    }
    
    
    void setFixedValue(std::string key, float value){
        EZPlugSynthMessage message;
        strcpy(message.target, key.c_str());
        message.value = value;
        message.messageType = SET_FIXED_VALUE;
        messageQueue.push_back(message);
    }
    
    float getFixedValue(std::string key){
        return v(key)->value;
    }
    
    void trigger(std::string key){
        EZPlugSynthMessage message;
        strcpy(message.target, key.c_str());
        message.messageType = TRIGGER;
        messageQueue.push_back(message);
    }
    
    // convert a midi number to a frequency
    float midiNumtoFreq(float midiNum){
        return CPPAudioUtil::midicps(midiNum);
    }
    
    
    inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
      
        for (int i = 0; i < sequencers.size(); i++) {
          sequencers.at(i)->advanceBySamples(frames.frames());
        }
        output->tick(frames);
        
        while (!messageQueue.empty()) {
            EZPlugSynthMessage msg = messageQueue.pop_front();
            switch (msg.messageType) {
                case SET_FIXED_VALUE:{
                    std::string target = msg.target;
                    pSetFixedValue(target, msg.value);
                    break;
                }
                case TRIGGER:{
                    std::string target = msg.target;
                    pTrigger(target);
                    break;
                }
            }
        }
        
        
        return frames;
    }
    
    // fill a buffer of floats, assuming the buffer is expecting max/min of 1,-1
    void fill16BitStereoBuffer(float *outData,  UInt32 numFrames){
        static const int blockSizeTimes2 = kSynthesisBlockSize * 2;
        
        
        for(int i = 0; i<numFrames * 2; i++){
            if(synthBufferReadPosition == 0){
                tick(outputFrames);
            }
            *outData++ = outputFrames[synthBufferReadPosition++];
            if(synthBufferReadPosition == blockSizeTimes2){
                synthBufferReadPosition = 0;
            }
        }
    }

    
};

#endif
