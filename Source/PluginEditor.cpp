/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ConvolvAudioProcessorEditor::ConvolvAudioProcessorEditor (ConvolvAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), openButton ("Open IR")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 100);
    openButton.onClick = [this] { chooseAndLoadIR(); };
    addAndMakeVisible(&openButton);
}

ConvolvAudioProcessorEditor::~ConvolvAudioProcessorEditor()
{
}

//==============================================================================
void ConvolvAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    /*g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);*/
}

void ConvolvAudioProcessorEditor::resized()
{
    openButton.setBounds(10,10,getWidth()-20, 30);
}

void ConvolvAudioProcessorEditor::chooseAndLoadIR()
{
  juce::FileChooser chooser ("Open Impulse Response (wav)", juce::File("~"), "*wav");
  if(chooser.browseForFileToOpen()){
    std::cout << chooser.getResult().getFullPathName() << "\n";
  }

  audioProcessor.updateIR(chooser.getResult().getFullPathName().toStdString());
}