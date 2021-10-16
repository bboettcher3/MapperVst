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

  std::function<void(int sigIdx)> onSignalDragging = nullptr;

  void paint(juce::Graphics&) override;
  void resized() override;

  void mouseMove(const juce::MouseEvent& e) override;
  void mouseDrag(const juce::MouseEvent& e) override;
  void mouseExit(const juce::MouseEvent& e) override;

  int getIdx() { return mIdx; }
  mpr_dev getDevice() { return mDevice; }
  void setDevice(mpr_dev device) { mDevice = device; }
  int getNumSigs();
  int getDragPosition();
  void setDoneDragging() {
    mDragSigIdx = -1;
    repaint();
  }

 private:
  static constexpr auto SIG_HEIGHT = 40;
  static constexpr auto BASE_COLOUR = 0xFF52B85F;
  static constexpr auto MAX_IDX = 20;
  static constexpr auto DEV_NAME_RATIO = .33;

  juce::String typeToString(mpr_type type);

  mpr_dev mDevice;  // libmapper device
  bool mIsSource;   // Is data source or dest
  int mIdx;         // Index in block column
  int mHoverSigIdx = -1; // Index of sig being hovered over
  int mDragSigIdx = -1; // Index of sig being dragged

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignalBlockComponent)
};
