/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/MapViewComponent.h"

#ifdef WIN32
#define _WINSOCKAPI_  // for winsock1/2 conflicts
#endif

//==============================================================================
/**
 */
class MapperVstAudioProcessorEditor : public juce::AudioProcessorEditor {
 public:
  MapperVstAudioProcessorEditor(MapperVstAudioProcessor&);
  ~MapperVstAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;

 private:
  // UI parameters
  static constexpr auto TOP_PANEL_HEIGHT = 80;
  static constexpr auto SIG_BLOCK_WIDTH = 300;
  static constexpr auto BG_COLOUR_1 = 0xff595959;
  static constexpr auto BG_COLOUR_2 = 0xff4d4d4d;

  // UI Components
  MapViewComponent mMapView;

  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  MapperVstAudioProcessor& audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapperVstAudioProcessorEditor)
};
