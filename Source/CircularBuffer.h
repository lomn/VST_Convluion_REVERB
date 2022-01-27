/*
  ==============================================================================

    CircularBuffer.h
    Created: 12 Nov 2021 3:46:18pm
    Author:  clem

  ==============================================================================
*/

#include <JuceHeader.h>

#pragma once

class CircularBuffer
{
public:
  CircularBuffer(size_t len);
  ~CircularBuffer();

  void pushItem(const float e);
  void pushBlock(const float* block, size_t size);
  
  float popItem();
  void  popBlock(float* output, size_t size);

  //Watch out this will move the writing index.
  void addBlockFromCurrentReadPos(const float* block, size_t size);

private:
  void writeIndIncr(unsigned int incr=1);
  void readIndIncr(unsigned int incr=1);

  float* buffer;
  size_t buffer_size;
  unsigned int readIndex;
  unsigned int writeIndex;
};