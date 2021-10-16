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
  auto namePanel = mIsSource ? r.removeFromLeft(r.getWidth() * DEV_NAME_RATIO) : r.removeFromRight(r.getWidth() * DEV_NAME_RATIO);
  g.drawText(devName, namePanel, juce::Justification::centred, true);
  g.drawRect(namePanel, 1);

  // Draw signals for this block's direction from device
  mpr_list sigs = mpr_dev_get_sigs(mDevice, mIsSource ? MPR_DIR_OUT : MPR_DIR_IN);
  int i = 0;
  while (sigs) {
    mpr_sig sig = *sigs;
    juce::String sigName = juce::String(mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, NULL));
    //juce::String sigType = juce::String("(") + typeToString(mpr_obj_get_type(sig)) + juce::String(")");
    auto sigPanel = r.removeFromTop(SIG_HEIGHT);
    if (i % 2) {
      g.setColour(bg.darker(0.1f));
      g.fillRect(sigPanel.reduced(1));
    }
    if (i == mHoverSigIdx) {
      g.setColour(bg.brighter(0.2f));
      g.fillRect(sigPanel.reduced(1));
    }
    g.setColour((i == mDragSigIdx) ? juce::Colours::white : juce::Colours::black);
    g.drawText(sigName, sigPanel, juce::Justification::centred);
    g.drawRect((i == mDragSigIdx) ? sigPanel.reduced(1) : sigPanel, 1);
    sigs = mpr_list_get_next(sigs);
    ++i;
  }
}

void SignalBlockComponent::resized() {}

void SignalBlockComponent::mouseMove(const juce::MouseEvent& e) { 
  if ((e.x < getWidth() * DEV_NAME_RATIO && mIsSource) || (e.x > getWidth() - getWidth() * DEV_NAME_RATIO && !mIsSource)) {
    mHoverSigIdx = -1;
    repaint();
  } else {
    int sigIdx = (int)(e.y / (float)SIG_HEIGHT);
    if (sigIdx != mHoverSigIdx) {
      mHoverSigIdx = sigIdx;
      repaint();
    }
  }
}
void SignalBlockComponent::mouseDrag(const juce::MouseEvent& e) {
  // Signal mapping drag
  if (mHoverSigIdx >= 0 && mDragSigIdx == -1 && mIsSource) {
    mDragSigIdx = mHoverSigIdx;
    if (onSignalDragging != nullptr) onSignalDragging(mHoverSigIdx);
    repaint();
  }
}
void SignalBlockComponent::mouseExit(const juce::MouseEvent& e) {
  mHoverSigIdx = -1;
  repaint();
}

int SignalBlockComponent::getNumSigs() {
  return mpr_list_get_size(mpr_dev_get_sigs(mDevice, mIsSource ? MPR_DIR_OUT : MPR_DIR_IN));
}

int SignalBlockComponent::getDragPosition() {
  if (mDragSigIdx == -1) return 0;
  return mDragSigIdx * SIG_HEIGHT + (SIG_HEIGHT / 2.0f);
}

juce::String SignalBlockComponent::typeToString(mpr_type type) {
  char typeChar = (char)type;
  DBG(typeChar);
  switch (type) { 
    case MPR_FLT:
      return "float";
    case MPR_DBL:
      return "double";
    case MPR_INT64:
      return "int64";
    case MPR_INT32:
      return "int32";
    case MPR_STR:
      return "string";
    case MPR_BOOL:
      return "bool";
    default:
      return "unknown";
  }
}