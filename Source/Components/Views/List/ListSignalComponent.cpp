/*
  ==============================================================================

    ListSignalComponent.cpp
    Created: 20 Jul 2022 8:59:09pm
    Author:  brady

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ListSignalComponent.h"

//==============================================================================
ListSignalComponent::ListSignalComponent(MapperManager::Signal* signal, juce::Colour colour)
    : mSignal(signal), mColour(colour) {
  mSigString = mSignal->getDescription(mShowsDetails);
}

ListSignalComponent::~ListSignalComponent() {}

void ListSignalComponent::paint(juce::Graphics& g) {
  auto r = getLocalBounds();
  g.setColour(mIsHovering ? mColour.brighter() : mColour);
  g.fillRect(r.withSizeKeepingCentre(r.getWidth() - 2, r.getHeight() - 2));

  g.setColour(juce::Colours::black);
  g.drawRect(getLocalBounds(), 1);

  g.setColour(juce::Colours::black);
  g.setFont(14.0f);
  g.drawText(mSigString, getLocalBounds(), juce::Justification::centred, true);
}

void ListSignalComponent::resized() {
}

void ListSignalComponent::setColour(juce::Colour colour) {
  mColour = colour;
  repaint();
}

void ListSignalComponent::setShowDetails(bool shouldShow) {
  mShowsDetails = shouldShow;
  mSigString = mSignal->getDescription(shouldShow);
  repaint();
}