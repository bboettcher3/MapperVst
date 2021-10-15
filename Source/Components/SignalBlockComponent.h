/*
  ==============================================================================

    SignalBlockComponent.h
    Created: 14 Oct 2021 9:18:39pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "mapper/mapper.h"

//==============================================================================
/*
 */
class SignalBlockComponent : public juce::Component {
 public:
  SignalBlockComponent(bool isSource, mpr_dev device, int idx);
  ~SignalBlockComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  mpr_dev getDevice() { return mDevice; }
  void setDevice(mpr_dev device) { mDevice = device; }
  int getNumSigs();

 private:
  static constexpr auto SIG_HEIGHT = 40;
  static constexpr auto BASE_COLOUR = 0xFF52B85F;
  static constexpr auto MAX_IDX = 20;

  mpr_dev mDevice;  // libmapper device
  bool mIsSource;   // Is data source or dest
  int mIdx;         // Index in block column

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignalBlockComponent)
};
