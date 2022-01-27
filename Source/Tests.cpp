/*
  ==============================================================================

    Tests.cpp
    Created: 30 Sep 2021 2:52:18pm
    Author:  clem

  ==============================================================================
*/

#include "Tests.h"
#include "stdio.h"
#include "stdlib.h"
#include "Convolver.h"
#include "CircularBuffer.h"
#include "ConvolverManager.h"
#include "AudioFile/AudioFile.h"

#include <JuceHeader.h>
#include <chrono>


//size is size of a and b ; out should be allocated so that it's size is at least the  one of a and b
void oldSchoolConv(float * a, float * b, float * out, size_t size)
{
	for(int i = 0; i<size*2; i++)
	{
		out[i] = 0;
	}

	for(int i = 0; i<size*2; i++)
	{
		for(int j = 0; j<=i; j++)
		{
			if(i-j < size && j < size)
			{
				out[i] += a[i-j]*b[j];
			}
		}
	}
}

Tests::Tests()
{
	return;
}

Tests::~Tests()
{
	return;
}

void convolverTests()
{
	float a[46] 		= {0.};
	float b[54] 		= {0.};
	float out[54+46] 	= {0};

	int dec = 5;
	b[dec] = 1.;
	a[0] = 1.;

	Convolver conv (54, 46, b);
	for(int i = 0; i<46; i++)
	{
		a[i] = ((float)(rand()%100))/100.;
		//b[i] = ((float)(rand()%100))/100.;
	}

	conv.processBlock(a, out);

	printf("out : %f , %f, %f \n\n", out[0], out[1], out[2]);
	for(int i = 0; i < 54+46; i++)
	{
		if(i-dec >= 0 && i-dec<46)
		{
			if(abs(out[i]-a[i-dec]) > 0.0001){ 
				printf("value problems at %i ; value %f\n", i, out[i]-a[i]); 
			}
		}
		else if (abs(out[i]) > 0.001)
		{
			printf("value problems at %i ; value %f\n", i, out[i]); 
		}
	}

	//-_-_-_-_-__-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
	//Now let's test this with random io
	size_t len = 512;

	float c[512] = {0.};
	float d[512] = {0.};
	float out2[1025] = {0.};
	float out3[1025] = {0.};

	for(int i = 0; i<len; i++)
	{
		c[i] = ((float)(rand()%100))/100.;
		d[i] = ((float)(rand()%100))/100.;
	}

	Convolver conv2(len,len, c);

	//profiling
	auto start = std::chrono::high_resolution_clock::now();
	conv2.processBlock(d, out2);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	//printf("\n\nit took %x microsec to convolve 512 block\n\n", duration.count());
	std::cout << std::endl << "took microsec to convolve 512 block " << duration.count() << "\n";
  

	oldSchoolConv(c, d, out3, len);

	for(int i = 0; i<1024; i++)
	{
		if(abs(out2[i] - out3[i]) > 0.001)
		{
			printf("value problems at %i ; value %f\n", i, out2[i]-out3[i]); 
		}
	}

	return;
}

void circularBuffTests()
{
	CircularBuffer* buff = new CircularBuffer((size_t)10);
	
	printf("\n\nTesting the circular buffer\n");


	//Checking correct behaviour when pushing popping a single elmt
	for(int i = 0; i<10; i++)
	{
		//buff->pushItem((float)i);
		buff->pushItem(i);
	}

	for(int i = 0; i<10; i++)
	{
		float tmp = buff->popItem();
		if(tmp - (float)i > 0.0001)
		{
			printf("Error when push pop single item : %i, %f\n", i, tmp);
		}
	}

	//Checking correct behaviour when push pop blocks
	float a[9] = {1.};
	float b[9] = {0.5};

	for(int i = 0; i < 9; i++)
	{
		a[i] = 1.;
		b[i] = 2.;
	}

	buff->pushBlock(a, 9);
	buff->popBlock(b, 9);

	for(int i = 0; i < 9; i++)
	{
		if(a[i] - b[i] > 0.0001)
		{
			printf("Error when push pop block at %i ; orig val %f ; out val %f\n", i, a[i], b[i]);
		}
	}

	buff->addBlockFromCurrentReadPos(a, 9);
	buff->addBlockFromCurrentReadPos(a, 9);
	for(int i = 0; i<11; i++)
	{
		printf("poping for the %ith time value %f\n", i, buff->popItem());
	}

	delete buff;
	return;
}

void managerTests()
{
	#define IR_SIZE 3042
	#define IN_SIZE 512

	float ir[IR_SIZE];
	float sig[IN_SIZE];
	float out[IN_SIZE];

	for(int i = 0; i<IR_SIZE; i++)
	{
		ir[i] = 0.;
	}
	for(int i = 0; i<IN_SIZE; i++)
	{
		sig[i] = static_cast<float>(i);
	}
	ir[256] = 1.;

	ConvolverManager* mana = new ConvolverManager(IR_SIZE, IN_SIZE, ir);

	mana->processBlock(sig, out);
	for(int i = 0; i < IN_SIZE; i++)
	{
		if(abs(out[i] - sig[i]) > 0.01)
		{
			//printf("Error wrong output while testing convolver manager || got %f ; expected %f\n", out[i], sig[i]);
		}
		else{
			//printf("index : %i ; value %f ; in_val %f\n", i, out[i], sig[i]);
		}
	}

	for(int i = 0; i<IN_SIZE; i++)
	{
		sig[i] = 511.-static_cast<float>(i);
	}
	mana->processBlock(sig, out);
	for(int i = 0; i < IN_SIZE; i++)
	{
		if(abs(out[i] - sig[i]) > 0.01)
		{
			//printf("Error wrong output while testing convolver manager || got %f ; expected %f\n", out[i], sig[i]);
		}
		else{
			//printf("index : %i ; value %f ; in_val %f\n", i, out[i], sig[i]);
		}
	}
	

	delete mana;
}

void openFileTest()
{
	AudioFile<float> irFile;
	irFile.load("~/Documents/audio_dev/Convolv/ir_short.wav");
	printf("Num Channels : %i ; Num samples : %i;\n",irFile.getNumChannels() ,irFile.getNumSamplesPerChannel());
	float* irFloat = new float[irFile.getNumSamplesPerChannel()];

	for(int i = 0; i < irFile.getNumSamplesPerChannel(); i++)
	{
		irFloat[i] = irFile.samples[0][i];
	}
	

	delete[] irFloat;
}

void overallTest()
{
	printf("Testing overall\n");
	float inBlock[512];
	float outBlock[512];

	AudioFile<float> irFile;
	irFile.load("~/Documents/audio_dev/Convolv/ir_unid.wav");
	printf("Num Channels : %i ; Num samples : %i;\n",irFile.getNumChannels() ,irFile.getNumSamplesPerChannel());
	float* irFloat = new float[irFile.getNumSamplesPerChannel()];

	for(int i = 0; i < irFile.getNumSamplesPerChannel(); i++)
	{
		irFloat[i] = irFile.samples[0][i];

		if(!std::isnormal(irFloat[i]))
		{
			printf("%i th element is not normal value is : %f\n", i, irFloat[i]);
		}
	}

	for(int i = 0; i < 512; i++)
	{
		inBlock[i] = static_cast<float>(i);
		outBlock[i] = 0;
	}

	ConvolverManager* convMana = new ConvolverManager(irFile.getNumSamplesPerChannel(), 512, irFloat);
	convMana->processBlock(inBlock, outBlock);


	for(int i = 0; i < 512; i++)
	{
		printf("out[%i] : %f\n", i, outBlock[i]);
	}
	
	delete convMana;
	delete[] irFloat;
	printf("finished testing overall\n");
}

void Tests::run()
{
	convolverTests();
	circularBuffTests();
	managerTests();
	openFileTest();
	overallTest();
}
