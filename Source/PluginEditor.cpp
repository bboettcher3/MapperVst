/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MapperVstAudioProcessorEditor::MapperVstAudioProcessorEditor(MapperVstAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), mListView(p.mapperManager) {
  addAndMakeVisible(mFileOpenSave);
  addAndMakeVisible(mViewSelect);
  addAndMakeVisible(mListView);

  p.mapperManager.refreshGraph();

  setSize(800, 500);
}

MapperVstAudioProcessorEditor::~MapperVstAudioProcessorEditor() {}

//==============================================================================
void MapperVstAudioProcessorEditor::paint(juce::Graphics& g) {
  g.setGradientFill(juce::ColourGradient::vertical(juce::Colour(BG_COLOUR_1), juce::Colour(BG_COLOUR_2), getLocalBounds()));
  g.fillAll();

  // Draw direction labels
  auto r = getLocalBounds();
  r.removeFromTop(TOP_PANEL_HEIGHT);
}

void MapperVstAudioProcessorEditor::resized() {
  auto r = getLocalBounds();
  // Remove top panel
  auto topPanel = r.removeFromTop(TOP_PANEL_HEIGHT);

  mFileOpenSave.setBounds(topPanel.removeFromLeft(getWidth() * 0.1f));
  mViewSelect.setBounds(topPanel.removeFromLeft(getWidth() * 0.2f));

  // List view
  mListView.setBounds(r);
}