/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "mapper/mapper.h"

#ifdef WIN32
#define _WINSOCKAPI_ //for winsock1/2 conflicts
#endif

//==============================================================================
/**
*/
class MapperVstAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MapperVstAudioProcessorEditor (MapperVstAudioProcessor&);
    ~MapperVstAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    mpr_graph mGraph;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MapperVstAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapperVstAudioProcessorEditor)
};
