//
//  FilePlay.h
//  MorganJayAppCinder
//
//  Created by Morgan Packard on 3/22/12.
//  Copyright (c) 2012 Spoonicopter. All rights reserved.
//


#ifndef MorganJayAppCinder_FilePlay_h
#define MorganJayAppCinder_FilePlay_h

#ifdef __APPLE__
#import <AudioToolbox/AudioToolbox.h>
#endif

#define xxxnonRealtime 1


#import <dispatch/dispatch.h>
#include "EZPlugTriggerable.h"
#include <algorithm>
#include "ORDLockFreeQueue.h"
#include "FileReadExtAudioFile.h"
#include "FilePlayFileRead.h"

using namespace std;

static const int framesPerBuffer = 4000;

class FilePlayFrames : public StkFrames{
public:
    const static int defaultStartFrame = -1;
    long readPosition; // one plus the index of the last frame read IN THE ENTIRE FILE. So 
    long startFrame; // which sample of the file does this buffer start on?
    long endFrame; // which sample of the file does the buffer end on?
    FilePlayFrames():startFrame(-1),endFrame(-1), readPosition(0){
      resize(framesPerBuffer, 2);
    }
    
    long framesLeft(){
      return endFrame - readPosition + 1;
    }
    
};

enum FilePlayBufferSourceMessageType {
  SwitchFile
};


static const int kEZPLGFilePlayMaxFileNameLength = 200;

struct FilePlayBufferSourceMessage {
    FilePlayBufferSourceMessageType type;
    char message[kEZPLGFilePlayMaxFileNameLength];
};

class FilePlayBufferSource{
  FileReadExtAudioFile* file;
  ORDLockFreeQueue<FilePlayFrames> bufferQueue; // from this to FilePlay
  ORDLockFreeQueue<FilePlayBufferSourceMessage> messageQueue; // from FilePlay to this
  static const int kBufferQueuLen = 30;
  FilePlayFrames* lastFrameFilled;
  int indexOfLastFrame;
  dispatch_source_t timer;
  int playbackStartFrame; // the first frame in the file we play back from. Usually zero, but can be set to start somewhere in the middle
   
  void fillQueue(){
    while(!messageQueue.empty()){
      FilePlayBufferSourceMessage* message = messageQueue.pop_front_pointer();
      if(message->type == SwitchFile){
        file->open(string(message->message));
        indexOfLastFrame = file->fileSize() - 1;
      }
      messageQueue.pop_front_pointer_commit();
    }
  
    while(!bufferQueue.full()){
      FilePlayFrames* currentBuffer = bufferQueue.expose_back();
      if(lastFrameFilled == 0 || lastFrameFilled->endFrame == indexOfLastFrame){
        currentBuffer->startFrame = playbackStartFrame;
      }else{
        currentBuffer->startFrame = (lastFrameFilled->endFrame + 1) % file->fileSize();
      }
      currentBuffer->endFrame = min<long>(currentBuffer->startFrame + framesPerBuffer - 1, indexOfLastFrame);
      currentBuffer->readPosition = currentBuffer->startFrame;
      file->read(*currentBuffer, currentBuffer->startFrame);
      lastFrameFilled = currentBuffer;
      bufferQueue.commit_back();

    }
  }
  
  public:
    FilePlayBufferSource(std::string path):
      bufferQueue(kBufferQueuLen),
      lastFrameFilled(0),
      playbackStartFrame(0)
    {
      
      file = new FileReadExtAudioFile(path);
      indexOfLastFrame = file->fileSize() - 1;
      fillQueue();
      dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
      timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
      
     if (timer)
      {
        dispatch_source_set_timer(timer, dispatch_walltime(NULL, 0), NSEC_PER_SEC / 10ull, NSEC_PER_SEC / 100ull);
        dispatch_source_set_event_handler(timer, ^{fillQueue();});
        dispatch_resume(timer);
      }else{
        printf("Error starting timer in FilePlayBufferSource\n");
        exit(0);
      }
    }
    
    ~FilePlayBufferSource(){
      dispatch_suspend(timer);
      delete file;
    }
    
    FilePlayFrames getNextBuffer(){
      if(bufferQueue.empty()){
        printf("FilePlayBufferSource. Warning. FilePlay isn't loading data fast enough to keep up with the demands placed on it.\n");
      }
      return bufferQueue.pop_front();
    }
    
    void switchToFile(string path){
      if(path.size() > kEZPLGFilePlayMaxFileNameLength){
        printf("WARNING!!!!! FilePlayBufferSource: attempt to load a fileName that's too long\n");
      }else{
        FilePlayBufferSourceMessage* message = messageQueue.expose_back();
        message->type = SwitchFile;
        strcpy(message->message, path.c_str());
        messageQueue.commit_back();
      }
    }
    
    size_t fileSize(){
      return file->fileSize();
    }
    
    int channels(){
      return file->channels();
    }
    
    void reset(float startPercent = 0){
      
    }
    
};

class FilePlay : public EZPlugTriggerable {
  int sampleStartPosition; // what's the first sample of the file we go back to when FilePlay is triggered
  std::string path; // the path to the file
  const int numChannels;
  FilePlayFrames currentBuffer;
  FilePlayBufferSource bufferSource;
  FixedValue* startPercent;
  
  public:
    FilePlay(std::string path):
    path(path),
    sampleStartPosition(0),
    numChannels(2),
    bufferSource(path){
      startPercent = new FixedValue(0);
      if(bufferSource.channels() != numChannels){
        printf("Error in FilePlay. Number of channels in file does not match expected number of channels\n");
        exit(0);
      }
      currentBuffer = bufferSource.getNextBuffer();
    }
    
  void switchToFile(string pathArg){
    if(path != pathArg){
      path = pathArg;
      bufferSource.switchToFile(pathArg);
    }
  }
  
  // 0-1 value
  FilePlay* setStartPercent(FixedValue* startPercentArg){
    delete startPercent;
    startPercent = startPercentArg;
    return this;
  }
  
  ~FilePlay(){
    delete startPercent;
  }

  inline StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ){
    if(currentBuffer.startFrame == FilePlayFrames::defaultStartFrame){
      // FilePlayBufferSource isn't keeping up. Zero out the frames to produce silence 
      memset(&frames(0,0), 0, sizeof(StkFloat) * frames.size());
    }else{
      int outBufferWriteStart = 0;
      
      while(outBufferWriteStart < frames.frames()){
        int numFramesToWrite = min<int>(currentBuffer.framesLeft(), frames.frames() - outBufferWriteStart);
        // TODO maybe move this logic in to FilePlayFrames
        int bufferReadStart = currentBuffer.readPosition - currentBuffer.startFrame;
        
        int bytesToWrite = sizeof(StkFloat) * numFramesToWrite * numChannels;
        StkFloat* writePointer = &frames(outBufferWriteStart, 0);
        StkFloat* readPointer = &((currentBuffer)(bufferReadStart, 0));
        
        memcpy(writePointer, readPointer, bytesToWrite); 
        currentBuffer.readPosition += numFramesToWrite;
        outBufferWriteStart += numFramesToWrite;
        
        if(currentBuffer.readPosition > currentBuffer.endFrame){
          currentBuffer = bufferSource.getNextBuffer();
        }
      
      }
    }
    
    return frames;
  }
  
  void trigger(){
    
  }
  
};


#endif



