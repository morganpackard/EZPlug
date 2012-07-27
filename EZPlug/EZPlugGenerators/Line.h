//
//  Line.h
//  Thicket II
//
//  Created by morganpackard on 11/16/11.
//  Copyright (c) 2011 Interval Studios. All rights reserved.
//

#ifndef Thicket_II_Line_h
#define Thicket_II_Line_h

#ifndef Line_HEAD
#define Line_HEAD

#include "Stk.h"
using namespace stk;
#include "Generator.h"
#include "Mixer.h"

enum LineState {
  RUNNING,
  DONE
};

class Line : public Generator{

private:
    LineState state;
    float start;
    float end;
    float time; // seconds
    float rate;
    float val;
    
    void calculateRate(){
        rate = (end - start) / (time * Stk::sampleRate());
    }
    
protected:
    
public:

    bool loop;
    
    Line(): loop(true), state(RUNNING), val(0){
    }
    ~Line(){
    }
    
    inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
    
        switch (state) {
        
            case RUNNING:
                for(int i = 0; i < frames.frames(); i++){
                    val += rate;
                    frames(i,0) = val;
                }
            break;
        
            case DONE:
                for(int i = 0; i < frames.frames(); i++){
                    frames(i,0) = end;
                }
            default:
            break;
            
        }
                    
        if(rate >= 0){
            if(val >= end){
                if(loop){
                    val = start;
                }else{
                    state = DONE;
                }
            }
        }else{
            if(val < end){
                if(loop){
                    val = start;
                }else{
                    state = DONE;
                }
            }
        }
    
        return frames;
    }
    
    Line* setStart(float startArg){
        start = startArg;
        calculateRate();
        return this;
    }
    
    Line* setEnd(float endArg){
        end = endArg;
        calculateRate();
        return this;
    }
    
    Line* setTime(float timeArg){
        time = timeArg;
        calculateRate();
        return this;
    }
    
    void trigger(){
        state = RUNNING;
        val = start;
    }

    
};

#endif



#endif
