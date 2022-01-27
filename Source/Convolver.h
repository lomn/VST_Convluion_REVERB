/*
  ==============================================================================

    Convolver.h
    Created: 26 Sep 2021 3:36:41pm
    Author:  clem

  ==============================================================================
*/

#pragma once

#include "fft_lib/AudioFFT.h"

class Convolver
{
public:
  Convolver(int irSize, int inSize, const float* ir);
  ~Convolver();

  int getSize();

  /** outSig should be the size of irSize+inSize i.e what getSize(); returns
   * */
  void processBlock(const float* inSig, float* outSig);

private:

  audiofft::AudioFFT fft;

  int inSize;
  int irSize;
  int fftOrder;
  float* ioBuffer;

  float* irFFTRe;
  float* irFFTIm;

  float* inFFTRe;
  float* inFFTIm;
};
