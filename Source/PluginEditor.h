/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ConvolvAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ConvolvAudioProcessorEditor (ConvolvAudioProcessor&);
    ~ConvolvAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    void chooseAndLoadIR();
    juce::TextButton openButton;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ConvolvAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConvolvAudioProcessorEditor)
};
