/*
  ==============================================================================

    SignalBlockComponent.cpp
    Created: 14 Oct 2021 9:18:39pm
    Author:  brady

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SignalBlockComponent.h"

//==============================================================================
SignalBlockComponent::SignalBlockComponent(bool isSource, mpr_dev device, int idx)
    : mIsSource(isSource), mDevice(device), mIdx(idx) {}

SignalBlockComponent::~SignalBlockComponent() {}

void SignalBlockComponent::paint(juce::Graphics& g) {
  // Fill
  juce::Colour bg = juce::Colour(BASE_COLOUR);
  g.setColour(bg.withHue(bg.getHue() + ((float)mIdx / MAX_IDX)));
  g.fillAll();

  // Outline
  g.setColour(juce::Colours::black);
  g.drawRect(getLocalBounds(), 1);

  auto r = getLocalBounds();

  // Draw device name
  g.setColour(juce::Colours::black);
  g.setFont(14.0f);
  juce::String devName = juce::String(mpr_obj_get_prop_as_str(mDevice, MPR_PROP_NAME, NULL));
  auto namePanel = mIsSource ? r.removeFromLeft(r.getWidth() / 3) : r.removeFromRight(r.getWidth() / 3);
  g.drawText(devName, namePanel, juce::Justification::centred, true);
  g.drawRect(namePanel, 1);

  // Draw signals for this block's direction from device
  mpr_list sigs = mpr_dev_get_sigs(mDevice, mIsSource ? MPR_DIR_OUT : MPR_DIR_IN);
  while (sigs) {
    mpr_sig sig = *sigs;
    juce::String sigName = juce::String(mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, NULL));
    juce::String sigType = juce::String(mpr_obj_get_prop_as_str(sig, MPR_PROP_TYPE, NULL));
    auto sigPanel = r.removeFromTop(SIG_HEIGHT);
    g.drawText(sigName + juce::String(" ") + sigType, sigPanel, juce::Justification::centred);
    g.drawRect(sigPanel, 1);
    sigs = mpr_list_get_next(sigs);
  }
}

void SignalBlockComponent::resized() {}

int SignalBlockComponent::getNumSigs() {
  return mpr_list_get_size(mpr_dev_get_sigs(mDevice, mIsSource ? MPR_DIR_OUT : MPR_DIR_IN));
}