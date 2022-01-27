/*
  ==============================================================================

    ConvolverManager.h
    Created: 26 Sep 2021 3:37:06pm
    Author:  clem

  ==============================================================================
*/

#pragma once

#include "Convolver.h"
#include "CircularBuffer.h"
#include <stdlib.h>



//Attention a implémenter = dans les classes utilisés par std vector
class ConvolverManager
{
public:
  ConvolverManager(int irSize, int inSize, const float* ir);
  ~ConvolverManager();

  /**
   * @brief Computes the convolution of the input signal with impulse response given at initialization.
   * This has memory of previously passed blocks, what is returned contains the tail of previous blocks + the beginning of the current block processed.
   * 
   * @param inSig input signal, should be of size inSize, set during initialization.
   * @param outSig output signal, of equal size as the input.
   */
  void processBlock(const float* inSig, float* outSig);

private:

  std::vector<Convolver*> convolvers;
  CircularBuffer* outBuffer;

  int irSize;
  int inSize;
};