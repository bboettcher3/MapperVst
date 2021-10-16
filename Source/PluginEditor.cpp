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

  setSize(800, 500);
}

MapperVstAudioProcessorEditor::~MapperVstAudioProcessorEditor() {
  // if (mDummyDevice) mpr_dev_free(mDummyDevice);
  if (mGraph) mpr_graph_free(mGraph);
  mSourceBlocks.clear();
  mDestBlocks.clear();
}

void MapperVstAudioProcessorEditor::timerCallback() {
  // Sync graph with network
  mpr_graph_poll(mGraph, 500);

  // Print devices
  mpr_list devs = mpr_graph_get_list(mGraph, MPR_DEV);
  while (devs) {
    mpr_dev dev = *devs;
    juce::String devName = juce::String(mpr_obj_get_prop_as_str(dev, MPR_PROP_NAME, NULL));
    // Update or create source blocks
    if (mpr_dev_get_sigs(dev, MPR_DIR_OUT)) {
      bool foundBlock = false;
      // Update signal block if already exists
      for (int i = 0; i < mSourceBlocks.size(); ++i) {
        if (mSourceBlocks[i]->getDevice() == dev) {
          foundBlock = true;
        }
      }
      if (!foundBlock) {
        // Create a new block if it doesn't exist yet
        auto newBlock = new SignalBlockComponent(true, dev, mSourceBlocks.size());
        newBlock->addMouseListener(this, false);
        // signal clicking lambda
        newBlock->onSignalDragging = [this, newBlock](int sigIdx) {
          mDragSource = newBlock;
        };
        addAndMakeVisible(newBlock);
        mSourceBlocks.add(newBlock);
      }
    }
    // Update or create dest blocks
    if (mpr_dev_get_sigs(dev, MPR_DIR_IN)) {
      bool foundBlock = false;
      // Update signal block if already exists
      for (int i = 0; i < mDestBlocks.size(); ++i) {
        if (mDestBlocks[i]->getDevice() == dev) {
          foundBlock = true;
        }
      }
      if (!foundBlock) {
        // Create a new block if it doesn't exist yet
        auto newBlock = new SignalBlockComponent(false, dev, mDestBlocks.size());
        addAndMakeVisible(newBlock);
        mDestBlocks.add(newBlock);
      }
    }
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
  // TODO: show number of signals instead of devices
  juce::String sourcesLabel = juce::String("Sources (") + juce::String(mSourceBlocks.size()) + juce::String(")");
  g.drawText(sourcesLabel, dirLabelPanel.removeFromLeft(dirLabelPanel.getWidth() / 2), juce::Justification::left);
  juce::String destsLabel = juce::String("Destinations (") + juce::String(mDestBlocks.size()) + juce::String(")");
  g.drawText(destsLabel, dirLabelPanel, juce::Justification::right);

  // Draw dragging arrow
  if (mDragSource != nullptr) {
    juce::Point<int> dragStart = mDragSource->getBounds().getTopRight().translated(0, mDragSource->getDragPosition());
    g.setColour(juce::Colours::white);
    g.drawArrow(juce::Line<int>(dragStart, mDragPoint).toFloat(), 4, 10, 10);
  }
}

void MapperVstAudioProcessorEditor::resized() {
  auto r = getLocalBounds();
  // Remove top panel
  r.removeFromTop(TOP_PANEL_HEIGHT);
  r.removeFromTop(DIR_LABEL_HEIGHT);

  // Position signal blocks
  // Source blocks
  auto sourcePanel = r.removeFromLeft(SIG_BLOCK_WIDTH);
  for (int i = 0; i < mSourceBlocks.size(); ++i) {
    mSourceBlocks[i]->setBounds(sourcePanel.removeFromTop(mSourceBlocks[i]->getNumSigs() * SIG_HEIGHT));
  }
  // Dest blocks
  auto destPanel = r.removeFromRight(SIG_BLOCK_WIDTH);
  for (int i = 0; i < mDestBlocks.size(); ++i) {
    mDestBlocks[i]->setBounds(destPanel.removeFromTop(mDestBlocks[i]->getNumSigs() * SIG_HEIGHT));
  }
}

void MapperVstAudioProcessorEditor::mouseUp(const juce::MouseEvent& e) {
  if (mDragSource != nullptr) {
    mDragSource->setDoneDragging();
    mDragSource = nullptr;
    mDragPoint = juce::Point<int>();
  }
}

void MapperVstAudioProcessorEditor::mouseDrag(const juce::MouseEvent& e) {
  if (mDragSource != nullptr) {
    mDragPoint = e.getEventRelativeTo(this).getPosition();
    repaint();
  }
}