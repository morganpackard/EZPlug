//
//  Enveloper.h
//  Thicket
//
//  Created by Morgan Packard on 6/9/11.
//  Copyright 2011 Interval Studios. All rights reserved.
//

// apply an envelope to a Generator

#ifndef Enveloper_HEAD
#define Enveloper_HEAD

#include "Stk.h"
using namespace stk;
#include "Generator.h"
#include "ADSR.h"
#include "AudioConfiguration.h"
#include "EZPlugTriggerable.h"
#include "FixedValue.h"

// the template class accepts a delegate

class CallbackEnveloperNullDelegate{
    public:
    void envFinished(){}
};

template <class T> class CallbackEnveloper : public EZPlugTriggerable{
    
//class Enveloper : public Generator{

private:
    bool doesNotSustain;

    ADSR* env;
    Generator* audioSource;
    StkFrames* envFrames;
    T* envelopeFinishedDelegate;
    bool envFinishedCalled;
    
    FixedValue* attackFV;
    FixedValue* decayFV;
    FixedValue* sustainFV;
    FixedValue* releaseFV;
    
    FixedValue defaultAudioSource;
    
    float lastA;
    float lastD;
    float lastS;
    float lastR;
    
    // use fixedValue objects rather than floats
    // these fixedValue objects must be watched so we can update when they change
    bool useFixedValues;
    
    void updateIfFixedValuesChanged(){
        if(attackFV->value != lastA ){
            env->setAttackTime(attackFV->value);
            lastA = attackFV->value;
        }
        if(decayFV->value != lastD ){
            env->setDecayTime(decayFV->value);
            lastD = decayFV->value;
        }
        if(sustainFV->value != lastS ){
            env->setSustainLevel(sustainFV->value);
            lastS = sustainFV->value;
        }
        if(releaseFV->value != lastR ){
            env->setReleaseTime(releaseFV->value);
            lastR = releaseFV->value;
        }
    }
    
protected:
    
public:
    
    void (T::*envFinished)();
    
    CallbackEnveloper():defaultAudioSource(1.0),audioSource(&defaultAudioSource){
        env = new ADSR();
        env->setAllTimes(0.01, 0.1, 0, 0.1);
        envFrames = new StkFrames(kSynthesisBlockSize, 1);
        envelopeFinishedDelegate = NULL;
        envFinishedCalled = false;
        doesNotSustain = false;
        
        // I think this is to force an update of the env the first time setAllTimes is called with FixedValue objects
        lastA = -1;
        lastS = -1;
        lastD = -1;
        lastR = -1;
        
        useFixedValues = false;
        
    }
    
    ~CallbackEnveloper(){
		delete env;
        delete envFrames;
    }
    
    inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
    
        if(useFixedValues){
            updateIfFixedValuesChanged();
        }
    
        env->tick(*envFrames);        
        audioSource->tick(frames);
        
        if(frames.channels() != 2 && frames.channels() != 1){
            printf("\n invalid number of frames in Enveloper.tick");
            exit(EXIT_FAILURE);
        }
                           
        #ifdef USE_APPLE_ACCELERATE
            for(int i = 0; i < frames.channels(); i++){
                vDSP_vmul(
                    &frames[i],                         //const float   __vDSP_input1[],
                    frames.channels(),                  //vDSP_Stride   __vDSP_stride1,
                    &(*envFrames)[0],                   //const float   __vDSP_input2[],
                    1,                                  //vDSP_Stride   __vDSP_stride2,
                    &frames[i],                         //float         __vDSP_result[],
                    frames.channels(),                  //vDSP_Stride   __vDSP_strideResult,
                    frames.frames()                     //vDSP_Length   __vDSP_size
                );
            }
        #else
            if(frames.channels() == 2){
                for(int i = 0; i < frames.frames(); i++){
                    frames(i, 0 ) *= (*envFrames)(i,0);
                    frames(i, 1) *=  (*envFrames)(i,0);
                }
            }else if(frames.channels() == 1){
                for(int i = 0; i < frames.frames(); i++){
                    frames(i, 0 ) *= (*envFrames)(i,0);
                }
            }
        #endif
        
        if(doesNotSustain && env->getState() == env->SUSTAIN){
            env->keyOff();
        }
        
        if (env->getState() == env->SUSTAIN && envelopeFinishedDelegate != NULL && !envFinishedCalled) {
            envFinishedCalled = true;
            (envelopeFinishedDelegate->*envFinished)();
        }
        
        return frames;
    }
    
    CallbackEnveloper* setAllTimes(StkFloat attack, StkFloat decay, StkFloat sustain, StkFloat release){
        if(useFixedValues){
            printf("\nOnce an Enveloper is using FixedValues, you can't call setAllTimes with floats\n");
            exit(EXIT_FAILURE);
        }
        env->setAllTimes(attack, decay, sustain, release);
        return this;
    }
    
    CallbackEnveloper* setAllTimes(FixedValue* attack, FixedValue* decay, FixedValue* sustain, FixedValue* release){
        attackFV = attack;
        decayFV = decay;
        sustainFV = sustain;
        releaseFV = release;
        useFixedValues = true;
        updateIfFixedValuesChanged();
        return this;
    }
    
    CallbackEnveloper* setRelease(float time){        
        if(useFixedValues){
            printf("\nOnce an Enveloper is using FixedValues, you can't set times with floats\n");
            exit(EXIT_FAILURE);
        }
        env->setReleaseTime(time);
        return this;
    }
    
    
    CallbackEnveloper* setDecay(float time){       
        if(useFixedValues){
            printf("\nOnce an Enveloper is using FixedValues, you can't set times with floats\n");
            exit(EXIT_FAILURE);
        }
        env->setDecayTime(time);
        return this;
    }
    
    CallbackEnveloper* setAudioSource(Generator* audioSourceArg){
        audioSource = audioSourceArg;
        return this;
    }
    
    void trigger(){
        envFinishedCalled = false;
        env->keyOff();
        env->keyOn();
    }
    
    CallbackEnveloper* setEnvFinishedDelegate(T* delegate){
        envelopeFinishedDelegate = delegate;
        return this;
    }
    
    
    CallbackEnveloper* setDoesNotSustain(bool doesNotSustainArg){
        doesNotSustain = doesNotSustainArg;
        return this;
    }
    
    float value(){
        return env->lastOut();

    }
    
    void keyOn(){
        env->keyOn();
    }
    
    void keyOff(){
        env->keyOff();
    }
    
    
    
};


typedef CallbackEnveloper<CallbackEnveloperNullDelegate> Enveloper;

#endif
