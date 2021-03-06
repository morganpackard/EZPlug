//
//  AutoMappedSequencer.h
//  ChuckWagonBoys
//
//  Created by Morgan Packard on 5/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ChuckWagonBoys_AutoMappedSequencer_h
#define ChuckWagonBoys_AutoMappedSequencer_h

#import <map>
#import "EZPlugSynth.h"

class AutoMappedSequencerStep {

  typedef std::map<std::string, bool> TriggersMapType;
  TriggersMapType triggers;
  
  
  typedef std::map<std::string, float> ValsMapType;
  ValsMapType values;

public:
  void set(EZPlugSynth* synth){
    for(TriggersMapType::iterator iterator = triggers.begin(); iterator != triggers.end(); iterator++) {
        if(iterator->second){
          synth->trigger(iterator->first);
        }
    }
    
    for(ValsMapType::iterator iterator = values.begin(); iterator != values.end(); iterator++) {
      synth->setFixedValue(iterator->first, iterator->second);
    }
  }
  
  void setTrigger(string triggerableName, bool triggerOrNot = true){
      triggers[triggerableName] = triggerOrNot;
  }
  
  bool doesTrigger(string triggerableName){
    if(triggers.find(triggerableName) == triggers.end()){
      return false;
    }else{
      return triggers[triggerableName];
    }
  }
  
  void setValue(string fixedValueName, float value){
    values[fixedValueName] = value;
  }
  
  const ValsMapType& getValsMap(){
    return values;
  }
  
};
typedef Sequencer<AutoMappedSequencerStep, EZPlugSynth> BasicSequencer;


#endif
