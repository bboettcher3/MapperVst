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
ListSignalComponent::ListSignalComponent(MapperManager::Signal& signal) : mSignal(signal) {
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
  mSigName = juce::String(mpr_obj_get_prop_as_str(mSignal.sig, MPR_PROP_NAME, nullptr));
}

ListSignalComponent::~ListSignalComponent()
{
}

void ListSignalComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText(mSigName, getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void ListSignalComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}