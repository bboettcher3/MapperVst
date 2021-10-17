/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MapperVstAudioProcessorEditor::MapperVstAudioProcessorEditor(MapperVstAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  mGraph = mpr_graph_new(MPR_OBJ);
  mpr_graph_add_cb(mGraph, graphCallbackHandler, MPR_OBJ, nullptr);
  startTimer(3000);

  addAndMakeVisible(mMapView);

  setSize(800, 500);
}

MapperVstAudioProcessorEditor::~MapperVstAudioProcessorEditor() {
  // if (mDummyDevice) mpr_dev_free(mDummyDevice);
  if (mGraph) mpr_graph_free(mGraph);
}

void MapperVstAudioProcessorEditor::timerCallback() {
  // Sync graph with network
  mpr_graph_poll(mGraph, 500);

  mMapView.clearDevices();
  mpr_list devs = mpr_graph_get_list(mGraph, MPR_DEV);
  while (devs) {
    mpr_dev dev = *devs;
    mMapView.addDevice(dev);
    devs = mpr_list_get_next(devs);
  }
  resized();
}

void MapperVstAudioProcessorEditor::graphCallbackHandler(mpr_graph g, mpr_obj o, mpr_graph_evt e, const void* v) {
  // DBG("called back\n");
}

//==============================================================================
void MapperVstAudioProcessorEditor::paint(juce::Graphics& g) {
  g.setGradientFill(juce::ColourGradient::vertical(juce::Colour(BG_COLOUR_1), juce::Colour(BG_COLOUR_2), getLocalBounds()));
  g.fillAll();

  // Draw direction labels
  auto r = getLocalBounds();
  r.removeFromTop(TOP_PANEL_HEIGHT);
  auto dirLabelPanel = r.removeFromTop(DIR_LABEL_HEIGHT);
  g.setColour(juce::Colours::white);
  // Show number of signals
  juce::String sourcesLabel = juce::String("Sources (") + juce::String(mMapView.getNumSourceSigs()) + juce::String(")");
  g.drawText(sourcesLabel, dirLabelPanel.removeFromLeft(dirLabelPanel.getWidth() / 2), juce::Justification::left);
  juce::String destsLabel = juce::String("Destinations (") + juce::String(mMapView.getNumDestSigs()) + juce::String(")");
  g.drawText(destsLabel, dirLabelPanel, juce::Justification::right);
}

void MapperVstAudioProcessorEditor::resized() {
  auto r = getLocalBounds();
  // Remove top panel
  r.removeFromTop(TOP_PANEL_HEIGHT);
  r.removeFromTop(DIR_LABEL_HEIGHT);

  // Map view
  mMapView.setBounds(r);
}