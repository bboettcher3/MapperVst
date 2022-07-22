/*
  ==============================================================================

    ListSignalComponent.h
    Created: 20 Jul 2022 8:59:09pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../../MapperManager.h"

//==============================================================================
/*
 */
class ListSignalComponent : public juce::Component {
 public:
  ListSignalComponent(MapperManager::Signal* signal, juce::Colour colour);
  ~ListSignalComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  MapperManager::Signal* getSignal() { return mSignal; }
  void setColour(juce::Colour colour);
  void setIsHovering(bool isHovering) {
    mIsHovering = isHovering;
    repaint();
  }

 private:
  // Bookkeeping
  MapperManager::Signal* mSignal;
  juce::Colour mColour;
  juce::String mSigName;
  bool mIsHovering = false;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListSignalComponent)
};
