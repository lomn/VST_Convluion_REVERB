/*
  ==============================================================================

    Convolver.cpp
    Created: 26 Sep 2021 3:36:41pm
    Author:  clem

  ==============================================================================
*/

#include "Convolver.h"
#include "fft_lib/AudioFFT.h"
#include <JuceHeader.h>

Convolver::Convolver(int irSize, int inSize, const float * ir)
{
  this->irSize = irSize;
  this->inSize = inSize;

  //Create and init the fft
  unsigned int convolvedSize = irSize + inSize - 1;

  this->fftOrder = pow(2, ceil(log(convolvedSize)/log(2)));

  fft.init(this->fftOrder);

  this->ioBuffer = new float[fftOrder];

  this->irFFTRe = new float[fftOrder];
  this->irFFTIm = new float[fftOrder];

  this->inFFTRe = new float[fftOrder];
  this->inFFTIm = new float[fftOrder];

  //calcul de fft pour l'ir puis storer la fft au bon endroit.
  for(int i = irSize; i<fftOrder; i++)
  {
    ioBuffer[i] = 0;
  }
  memcpy(ioBuffer, ir, irSize*sizeof(float));

  fft.fft(ioBuffer, irFFTRe, irFFTIm);


  /*printf("\nconvolved size : %i\n", convolvedSize);
  printf("fft order : %i\n", fftOrder);
  printf("nec im/re size : %i\n\n", fft.ComplexSize(fftOrder));*/
}

Convolver::~Convolver()
{
  //delete this->fft;

  delete[] this->ioBuffer;

  delete[] this->irFFTRe;
  delete[] this->irFFTIm;

  delete[] this->inFFTRe;
  delete[] this->inFFTIm;
}
void Convolver::processBlock(const float* inSig, float* outSig)
{
  for(int i = inSize; i < fftOrder; i++)
  {
    ioBuffer[i] = 0.;
  }

  memcpy(ioBuffer, inSig, inSize*sizeof(float));

  fft.fft(ioBuffer, inFFTRe, inFFTIm);

  for(int i = 0; i < fftOrder; i++)
  {
    float inRe = inFFTRe[i];
    float inIm = inFFTIm[i];

    //(a+ib)*(x+iy)=ax-by+i(ay+bx)
    inFFTRe[i] = inRe*irFFTRe[i] - inIm*irFFTIm[i];
    inFFTIm[i] = inIm*irFFTRe[i] + inRe*irFFTIm[i];
  }

  fft.ifft(ioBuffer, inFFTRe, inFFTIm);

  memcpy(outSig, ioBuffer, sizeof(float)*this->getSize());

  return;
}

int Convolver::getSize()
{
  return this->inSize+this->irSize;
}