/*
  ==============================================================================

    CircularBuffer.cpp
    Created: 12 Nov 2021 3:46:18pm
    Author:  clem

  ==============================================================================
*/

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(size_t len): readIndex(0), writeIndex(0), buffer_size(len)
{
  buffer = new float[len];

  for(int i = 0; i<len; i++)
  {
    buffer[i] = 0.;
  }
}

void CircularBuffer::writeIndIncr(unsigned int incr)
{
  writeIndex = (writeIndex+incr) % buffer_size;
}

void CircularBuffer::readIndIncr(unsigned int incr)
{
  readIndex = (readIndex+incr) % buffer_size;
}

CircularBuffer::~CircularBuffer()
{
  delete[] buffer;
}

void CircularBuffer::pushItem(const float e)
{
  buffer[writeIndex] = e;
  writeIndIncr();
}

void CircularBuffer::pushBlock(const float* block, size_t size)
{
  for(int i = 0; i < size; i++)
  {
    buffer[(writeIndex+i)%buffer_size] = block[i];
  }
  writeIndIncr(size);
}

float CircularBuffer::popItem()
{
  float out = buffer[readIndex];
  buffer[readIndex] = 0.;
  readIndIncr();
  return out;
}

void CircularBuffer::popBlock(float* output, size_t size)
{
  for(int i = 0; i < size; i++)
  {
    output[i] = buffer[(i+readIndex)%buffer_size];
    buffer[(i+readIndex)%buffer_size] = 0.;
  }

  readIndIncr(size);
}

void CircularBuffer::addBlockFromCurrentReadPos(const float* block, size_t size)
{
  if(size>=buffer_size){while(1){}}

  for(int i = 0; i<size; i++)
  {
    buffer[(i+readIndex)%buffer_size] += block[i];
  }
}