/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Tests.h"
#include "AudioFile/AudioFile.h"
//==============================================================================
ConvolvAudioProcessor::ConvolvAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	Tests tester;
	tester.run();
    
    isIRBeingLoad = true;
    float zeros[2] = {0.,0.};
    convManaL = new ConvolverManager(1,1,zeros);
    convManaR = new ConvolverManager(1,1,zeros);
    irPath = "nap";
}

ConvolvAudioProcessor::~ConvolvAudioProcessor()
{
    delete convManaL;
    delete convManaR;
}

//==============================================================================
const juce::String ConvolvAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ConvolvAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ConvolvAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ConvolvAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ConvolvAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ConvolvAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ConvolvAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ConvolvAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ConvolvAudioProcessor::getProgramName (int index)
{
    return {};
}

void ConvolvAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ConvolvAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    sR = sampleRate;
    blockLen = samplesPerBlock;

    if(irPath != "nap")
    {
        updateIR(irPath);
    }
    else
    {
        delete convManaL;
        delete convManaR;

        float id[2] = {1.,0.};
        convManaL = new ConvolverManager(1, samplesPerBlock, id);
        convManaR = new ConvolverManager(1, samplesPerBlock, id);
    }
}

void ConvolvAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ConvolvAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ConvolvAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    /*for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }*/
    //const float * input = buffer.getReadPointer(0);
    if(!isIRBeingLoad)
    {
        convManaL->processBlock(buffer.getReadPointer(0), buffer.getWritePointer(0));
        convManaR->processBlock(buffer.getReadPointer(1), buffer.getWritePointer(1));
    }

}

//==============================================================================
bool ConvolvAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ConvolvAudioProcessor::createEditor()
{
    return new ConvolvAudioProcessorEditor (*this);
}

//==============================================================================
void ConvolvAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ConvolvAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ConvolvAudioProcessor();
}

void ConvolvAudioProcessor::updateIR(std::string newIRPath)
{
    isIRBeingLoad = true;
    //update the paths
    this->irPath = newIRPath;


    //prepare to update the convolvers
    delete convManaL;
    delete convManaR;


    //open up the IR
    AudioFile<float> irFile;
    irFile.load(irPath);
    
    if(static_cast<double>(irFile.getSampleRate())-this->sR > 1)
    {
        std::cout << "[-] sample rate of ir and playback is different Continuing anyways\n";
    }

    //printf("[+] Num Channels : %i ; Num samples : %i;\n", irFile.getNumChannels(), irFile.getNumSamplesPerChannel());
	float* irFloat = new float[irFile.getNumSamplesPerChannel()];

	for(int i = 0; i < irFile.getNumSamplesPerChannel(); i++)
	{
		irFloat[i] = irFile.samples[0][i];
	}
	
    convManaL = new ConvolverManager(irFile.getNumSamplesPerChannel(), blockLen, irFloat);
    convManaR = new ConvolverManager(irFile.getNumSamplesPerChannel(), blockLen, irFloat);

	delete[] irFloat;
    isIRBeingLoad = false;
}