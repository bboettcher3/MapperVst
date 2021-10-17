/*
  ==============================================================================

    MapViewComponent.h
    Created: 16 Oct 2021 9:46:45pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "mapper/mapper.h"

//==============================================================================
/*
 */
class MapViewComponent : public juce::Component {
 public:
  MapViewComponent();
  ~MapViewComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  void mouseMove(const juce::MouseEvent& e) override;
  void mouseDrag(const juce::MouseEvent& e) override;
  void mouseDown(const juce::MouseEvent& e) override;
  void mouseUp(const juce::MouseEvent& e) override;
  void mouseExit(const juce::MouseEvent& e) override;

  void addDevice(mpr_dev device);
  void clearDevices() {
    mSourceSigs.clear();
    mDestSigs.clear();
    mNumDevices = 0;
  }

  int getNumSourceSigs() { return mSourceSigs.size(); }
  int getNumDestSigs() { return mDestSigs.size(); }

 private:
  static constexpr auto BASE_COLOUR = 0xFF52B85F;
  static constexpr auto MAX_IDX = 20;
  static constexpr auto DEFAULT_DEV_NAME_WIDTH = 100;
  static constexpr auto SIG_HEIGHT = 40;
  static constexpr auto MAPPING_GAP = 200;

  typedef struct Signal {
    Signal(): sig(nullptr) {}
    Signal(mpr_sig sig, int yPos, juce::Colour colour) : sig(sig), yPos(yPos), colour(colour) {}
    mpr_sig sig;
    int yPos;
    juce::Colour colour;
  } Signal;
  
  // Bookkeeping
  std::vector<Signal> mSourceSigs;
  std::vector<Signal> mDestSigs;
  Signal mDragSource;  // Only non-null sig if currently dragging
  Signal mHoverSig;  // Index of sig being hovered over
  juce::Point<int> mDragPoint;
  int mDevNameWidth = DEFAULT_DEV_NAME_WIDTH;
  int mSigWidth; // Adjusted in resized() relative to dev name width
  int mNumDevices = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapViewComponent)
};
