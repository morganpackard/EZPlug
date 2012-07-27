//
//  ORDCircularBuffer.h
//  conveyormarimba
//
//  Created by Morgan Packard on 2/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ORDCircularBuffer_h
#define ORDCircularBuffer_h

#pragma once

#include <cstdio>
#include <libkern/OSAtomic.h>


template <typename T>
class ORDLockFreeQueue
{
private:
    int QUEUESIZE;
    T*  q;                  // body of queue
    int qsize;
    int first;              // position of first element
    int last;               // position of last element
    int count;              // number of queue elements


public:
    ORDLockFreeQueue(const int size = 256) :
        QUEUESIZE(size), first(0), last(size - 1), count(0)
    {
        q = new T[size + 1];
    }

    ~ORDLockFreeQueue()
    {
        delete q;
    }
    
    void push_back(const T& t)
    {
        volatile int32_t tempLast;

        if (count >= QUEUESIZE)
        {
            printf("ORDLockFreeQueue Warning: queue overflow enqueue %d\n", QUEUESIZE);
        }
        else
        {
            OSAtomicCompareAndSwap32Barrier(tempLast, last, &tempLast);
            tempLast = (tempLast + 1) % QUEUESIZE;
            OSAtomicCompareAndSwap32Barrier(last, tempLast, &last);

            OSMemoryBarrier();
            q[last] = t;
            OSAtomicIncrement32Barrier(&count);
        }
    }

    // using pointers in order to avoid construction of new objects
    T* expose_back(){        
      volatile int32_t tempLast;

      if (count >= QUEUESIZE)
      {
          printf("ORDLockFreeQueue Warning: queue overflow enqueue %d\n", QUEUESIZE);
      }
      else
      {
          OSAtomicCompareAndSwap32Barrier(tempLast, last, &tempLast);
          tempLast = (tempLast + 1) % QUEUESIZE;
          OSAtomicCompareAndSwap32Barrier(last, tempLast, &last);

          OSMemoryBarrier();
      }
      return &q[last];
    }
    
    // call after expose_back
    void commit_back(){
      OSAtomicIncrement32Barrier(&count);
    }

    T* pop_front_pointer(){
        volatile int32_t tempFirst;
        T* x;

        if (count <= 0)
            printf("ORDLockFreeQueue Warning: empty queue dequeue.\n");
        else
        {
            OSMemoryBarrier();
            x = &q[first];

            OSAtomicCompareAndSwap32Barrier(tempFirst, first, &tempFirst);
            tempFirst = (tempFirst + 1) % QUEUESIZE;
            OSAtomicCompareAndSwap32Barrier(first, tempFirst, &first);
            
        }

        return x;
    }
    
    void pop_front_pointer_commit(){
        OSAtomicDecrement32Barrier(&count);
    }

    T pop_front()
    {
        volatile int32_t tempFirst;
        T x;

        if (count <= 0)
            printf("ORDLockFreeQueue Warning: empty queue dequeue.\n");
        else
        {
            OSMemoryBarrier();
            x = q[first];

            OSAtomicCompareAndSwap32Barrier(tempFirst, first, &tempFirst);
            tempFirst = (tempFirst + 1) % QUEUESIZE;
            OSAtomicCompareAndSwap32Barrier(first, tempFirst, &first);
            
            OSAtomicDecrement32Barrier(&count);
        }

        return x;
    }

    bool empty()
    {
        if (count <= 0) return true;
        else return false;
    }
    
    T front()
    {
        OSMemoryBarrier();
        return q[first];
    }

    void clear()
    {
        first = 0;
        last = QUEUESIZE - 1;
        count = 0;
    }
    
    size_t size()
    {
        return count;
    }
    
    bool full(){
      return count >= QUEUESIZE;
    }
};

#endif
