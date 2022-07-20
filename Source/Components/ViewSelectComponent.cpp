/*
  ==============================================================================

    ViewSelectComponent.cpp
    Created: 20 Jul 2022 5:06:27pm
    Author:  brady

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ViewSelectComponent.h"
#include "BinaryData.h"

//==============================================================================
ViewSelectComponent::ViewSelectComponent() {
  juce::Image listViewIcon = juce::PNGImageFormat::loadFrom(BinaryData::listViewIcon_png, BinaryData::listViewIcon_pngSize);
  mBtnListView.setImages(false, true, true, listViewIcon, 1.0f, juce::Colours::white, listViewIcon, 1.0f,
                         juce::Colours::lightgrey, listViewIcon, 1.0f, juce::Colours::black);
  mBtnListView.setClickingTogglesState(true);
  mBtnListView.setToggleable(true);
  mBtnListView.setToggleState(true, juce::dontSendNotification);
  addAndMakeVisible(mBtnListView);
}

ViewSelectComponent::~ViewSelectComponent() {}

void ViewSelectComponent::paint(juce::Graphics& g) {
  g.setColour(juce::Colours::white);
  g.drawText("View", getLocalBounds().removeFromTop(20), juce::Justification::centred);

  g.setColour(juce::Colours::white);
  g.drawRoundedRectangle(getLocalBounds().toFloat(), 8, 1.0f);
}

void ViewSelectComponent::resized() {
  auto r = getLocalBounds().withSizeKeepingCentre(getWidth() - PADDING, getHeight() - PADDING);

  mBtnListView.setBounds(r.removeFromLeft(ICON_SIZE));
}
