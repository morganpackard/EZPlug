#ifndef CPPAudioUtil_head
#define CPPAudioUtil_head

/*
 *  CPPAudioUtil.h
 *  BallOfString
 *
 *  Created by Morgan Packard on 1/9/11.
 *  Copyright 2011 intervalstudios. All rights reserved.
 *
 */

#include <string>
#include <math.h>
#include <string>
#include <vector>

class CPPAudioUtil{

public:
    
    
	static inline float midicps(float note)
	{
		return (float)440. * pow((float)2., (note - (float)69.) * (float)0.083333333333);
	}
    
    static inline float cpsmidi(float freq){
        return log2f(freq * (float)0.0022727272727) * (float)12. + (float)69.;
    }
    
    
    
	static inline float randomRange(float n1, float n2)
	{
		float r = rand() % 10000 / 10000.0;
		return (r*(n2-n1))+n1;
	}

    static inline float noteNameToMidiNum(std::string noteName){
        static const float A_ZERO_MIDI_NUM = 21;
        static const std::string midiNoteNames[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
        for(int i = 0; i < 12; i++){
            int len = midiNoteNames[i].find("#") == std::string::npos ? 1 : 2;
            if (noteName.substr(0, len) == midiNoteNames[i]) {
                float octave = atoi((noteName + "0").substr(len, 1).c_str()); // add 0 as the octave. If there's already an octave, "0" will be ignored.
                return i + (octave * 12) + A_ZERO_MIDI_NUM;
            }
        }
        return -1;
    }
    
    static inline float noteNameToFreq(std::string noteName){
        return midicps(noteNameToMidiNum(noteName));
    }
    
    
    // given a single value and a list of values (a scale), return the member of the list which is closest to the single value 
    static inline float snap(float number, std::vector<float>* targets){
        float ret = 0;
        float leastDistance = -1;
        for(int i = 0; i<targets->size(); i++){
            float distance = targets->at(i) - number;
            if(distance < 0){
                distance = -distance;
            }
            if(leastDistance == -1){
                leastDistance = distance;
            }
            if(distance <= leastDistance){
                leastDistance = distance;
                ret = targets->at(i);
            }
        }
        return ret;
    }
    
    // snap which wraps around 12 (octave in midi notes). 
    // todo -- maybe have it examine the scale and snap around multi-octave scales?
    static inline float octaveSnap(float number, std::vector<float>* targets){
        static const int NOTES_PER_OCTAVE = 12;
        int octave = number / NOTES_PER_OCTAVE;
        float baseNumber = number - (octave * NOTES_PER_OCTAVE);
        //float ret = snap(baseNumber, targets) + (octave * NOTES_PER_OCTAVE);
        return snap(baseNumber, targets) + (octave * NOTES_PER_OCTAVE);
    }

};

#endif