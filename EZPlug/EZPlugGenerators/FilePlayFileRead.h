//
//  FilePlayFileRead.h
//  ChuckWagonBoys
//
//  Created by Morgan Packard on 5/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ChuckWagonBoys_FilePlayFileRead_h
#define ChuckWagonBoys_FilePlayFileRead_h

#include "FileRead.h"
#include "FileReadExtAudioFile.h"
#include "Stk.h"

class FilePlayFileRead{
  public:
  virtual void open(string path) = 0;
  virtual void close() = 0;
  virtual void read( StkFrames& buffer, unsigned long startFrame = 0, bool doNormalize = true ) = 0;
  virtual unsigned long fileSize( void ) = 0;
  virtual unsigned int channels( void ) = 0;
  virtual ~FilePlayFileRead(){};
};

class FilePlayFileReadPCM : public FilePlayFileRead{
  FileRead fileRead;
  public:
  
    FilePlayFileReadPCM(string path):fileRead(path){}
    
    void open(string path){
      fileRead.open(path);
    }
    void close(){
      fileRead.close();
    }
    void read( StkFrames& buffer, unsigned long startFrame = 0, bool doNormalize = true ){
      fileRead.read( buffer, startFrame, doNormalize);
    }
    
    unsigned long fileSize( void ){
        return fileRead.fileSize();
    }
    unsigned int channels( void ){
      return fileRead.channels();
    };
};

class FilePlayFileReadMP3 : public FilePlayFileRead{
  FileReadExtAudioFile fileRead;
  public:
  FilePlayFileReadMP3(string path):fileRead(path){}
    
    void open(string path){
      fileRead.open(path);
    }
    void close(){
      fileRead.close();
    }
    void read( StkFrames& buffer, unsigned long startFrame = 0, bool doNormalize = true ){
      fileRead.read( buffer, startFrame, doNormalize);
    }
    unsigned long fileSize( void ){
      return fileRead.fileSize();
    }
    
    unsigned int channels( void ){
      return fileRead.channels();
    };
    
};

class FilePlayFileReadFactory {
public:
  FilePlayFileRead* createFileRead(std::string pathToFile){
  
    vector<string> compressedTypes;
    compressedTypes.push_back(".MP3");
    compressedTypes.push_back(".mp3");
    compressedTypes.push_back(".caf");
    
    bool isCompressed = false;
    
    for(size_t i = 0; i < compressedTypes.size(); i++){
      if(pathToFile.find(compressedTypes.at(i)) != std::string::npos){
        isCompressed = true;
      }
    }
  
    if(isCompressed){
      return new FilePlayFileReadMP3(pathToFile);
    }else{
      return new FilePlayFileReadPCM(pathToFile);
    }
  }
};

#endif
