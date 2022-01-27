/*
  ==============================================================================

    ConvolverManager.cpp
    Created: 26 Sep 2021 3:37:06pm
    Author:  clem

  ==============================================================================
*/

#include "ConvolverManager.h"


ConvolverManager::ConvolverManager(int irSize, int inSize, const float* ir)
{
  this->inSize = inSize;
  this->irSize = irSize;

  //For now we use blocks of size 512
  unsigned int num_ir = std::ceil(((float)irSize)/512.);
  std::cout << "num ir " << num_ir << "\n";

  float* irBuffer = new float[num_ir*512];

  for(int i = 0; i < num_ir*512; i++)
  {
    irBuffer[i]=0.;
  }

  memcpy(irBuffer, ir, irSize*sizeof(float));

  //Should be tweaked to accept irLen that are non multiple of 512
  for(unsigned int i = 0; i<num_ir; i++)
  {
    convolvers.push_back(new Convolver(512, inSize, &irBuffer[512*i]));
  }

  outBuffer = new CircularBuffer(num_ir*convolvers[0]->getSize()+1);

  //printf("num_ir %i, covlovers.size() %i\n", num_ir, convolvers.size());


  delete[] irBuffer;
}

ConvolverManager::~ConvolverManager()
{
  for (std::vector<Convolver *>::iterator i = convolvers.begin(); i != convolvers.end(); ++i) {
    delete *i;
  }
  delete outBuffer;
}

void ConvolverManager::processBlock(const float* inSig, float* outSig)
{
  if(convolvers.size() <= 0)
  {
    printf("halalahla\n");
  }
  

  const int blockOutSize = convolvers[0]->getSize();
  float * conv0 = new float[blockOutSize];
  convolvers[0]->processBlock(inSig, conv0);
  

  size_t tail_size = irSize;

  //tmp0 size isn't optimal here but making exact computations for its lenght seems like a pain in the arse ?
  float * tmp0 = new float[blockOutSize*convolvers.size()];
  float * convolved = new float[blockOutSize];
  float * iBuff = new float[inSize];

  for(int i = 0; i < inSize; i++)
  {
    iBuff[i] = inSig[i];
  }

  //tmp0 buffer will be used to store the tail fo the convolution
  for(int i = 0; i < tail_size; i++)
  {
    tmp0[i] = 0.;
  }

  //conv0 contains both the first block to be outputed but also 
  for(int i = 0; i < blockOutSize-inSize; i++)
  {
    tmp0[i] = conv0[i+inSize];
  }

  int nc = convolvers.size();
  //compute each convolutions and overlapp and add them on tthe tmp0 buffer
  //so that eventually we can overlapp and add them on the main circular buffer
  for(int i = 1; i < nc; i++)
  {
    convolvers[i]->processBlock(iBuff, convolved);
    for(int j = 0; j < blockOutSize; j++)
    {
      tmp0[(i-1)*blockOutSize+j] += convolved[j];
    }
  }

  //we now need to pop the block that overlapps the beginning of the conv0 output
  //so that we can add it to the current output.
  float * tmp1 = new float[inSize];
  outBuffer->popBlock(tmp1, inSize);
  for(int i = 0; i < inSize; i++)
  {
    conv0[i] += tmp1[i];
  }

  memcpy(outSig, conv0, this->inSize*sizeof(float));

  //The add part should be done in last since we first need to pop the tail part that was saved from the previous convolutions so that it can be added to the directly computed tail.
  //If any of that makes sense.
  outBuffer->addBlockFromCurrentReadPos(tmp0, irSize);

  delete[] tmp1;
  delete[] tmp0;
  delete[] conv0;
  delete[] convolved;

  return;
}