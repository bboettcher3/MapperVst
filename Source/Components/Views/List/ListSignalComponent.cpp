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
  // In your constructor, you should add any child components, and
  // initialise any special settings that your component needs.
  mSigName = juce::String(mpr_obj_get_prop_as_str(mSignal->sig, MPR_PROP_NAME, nullptr));
}

ListSignalComponent::~ListSignalComponent() {}

void ListSignalComponent::paint(juce::Graphics& g) {
  auto r = getLocalBounds();
  g.setColour(mIsHovering ? mColour.brighter() : mColour);
  g.fillRect(r.withSizeKeepingCentre(r.getWidth() - 2, r.getHeight() - 2));  // clear the background

  g.setColour(juce::Colours::black);
  g.drawRect(getLocalBounds(), 1);  // draw an outline around the component

  g.setColour(juce::Colours::black);
  g.setFont(14.0f);
  g.drawText(mSigName, getLocalBounds(), juce::Justification::centred, true);
}

void ListSignalComponent::resized() {
  // This method is where you should set the bounds of any child
  // components that your component contains..
}

void ListSignalComponent::setColour(juce::Colour colour) {
  mColour = colour;
  repaint();
}