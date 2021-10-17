/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "mapper/mapper.h"
#include "Components/MapViewComponent.h"

#ifdef WIN32
#define _WINSOCKAPI_  // for winsock1/2 conflicts
#endif

//==============================================================================
/**
 */
class MapperVstAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer {
 public:
  MapperVstAudioProcessorEditor(MapperVstAudioProcessor&);
  ~MapperVstAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;

  void timerCallback() override;

 private:
  // UI parameters
  static constexpr auto TOP_PANEL_HEIGHT = 80;
  static constexpr auto DIR_LABEL_HEIGHT = 20;
  static constexpr auto SIG_BLOCK_WIDTH = 300;
  static constexpr auto BG_COLOUR_1 = 0xff595959;
  static constexpr auto BG_COLOUR_2 = 0xff4d4d4d;

  static void graphCallbackHandler(mpr_graph g, mpr_obj o, mpr_graph_evt e, const void* v);

  mpr_graph mGraph; // libmapper graph that keeps track of network

  // UI Components
  MapViewComponent mMapView;

  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  MapperVstAudioProcessor& audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapperVstAudioProcessorEditor)
};
