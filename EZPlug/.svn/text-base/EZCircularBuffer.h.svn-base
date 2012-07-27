//
//  CircularBuffer.h
//  Thicket
//
//  Created by Morgan Packard on 7/11/11.
//  Copyright 2011 Interval Studios. All rights reserved.
//

// todo This is a totally not thread-safe piece of crap. Replace it with something that is thread safe

typedef unsigned int INT8U;
template<class KeyType, int size> class EZCircularBuffer{

private:
    INT8U writePointer; /**< write pointer */
    INT8U readPointer;  /**< read pointer */
    KeyType keys[size];    /**< Element of ciruclar buffer */

public:
    
    EZCircularBuffer(){
        writePointer = 0;
        readPointer  = 0;
    }
    
    bool isFull(){
        return ((writePointer + 1) % size) == readPointer;
    }
    
    bool isEmpty(){
     return readPointer == writePointer;
    }
    
    KeyType* nextWritePosition(){
        KeyType* ret =  &keys[writePointer];
        
        bool isFullTemp = isFull();
        if(!isFullTemp) 
        {
            writePointer++;
            writePointer %= size;
        }
        
        return ret;
    }

    
    KeyType* dequeue()
    {
        KeyType* ret;
        int isEmptyTemp =  isEmpty();
        if(!isEmptyTemp) 
        {
            ret = &keys[readPointer];
            readPointer++;
            readPointer %= size;
        }
        return ret;
    }
    
    int getSize(){
        return size;
    }
    
    KeyType* data(){
        return keys;
    }
    
    
};

