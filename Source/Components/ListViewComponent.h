/*
  ==============================================================================

    MapViewComponent.h
    Created: 16 Oct 2021 9:46:45pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../MapperManager.h"

//==============================================================================
/*
 */
class ListViewComponent : public juce::Component {
 public:
  ListViewComponent(MapperManager manager);
  ~ListViewComponent() {}

  void paint(juce::Graphics&) override;
  void resized() override;

  void mouseMove(const juce::MouseEvent& e) override;
  void mouseDrag(const juce::MouseEvent& e) override;
  void mouseDown(const juce::MouseEvent& e) override;
  void mouseUp(const juce::MouseEvent& e) override;
  void mouseExit(const juce::MouseEvent& e) override;

 private:
  static constexpr auto DIR_LABEL_HEIGHT = 20;
  static constexpr auto DEFAULT_DEV_NAME_WIDTH = 100;
  static constexpr auto SIG_HEIGHT = 40;
  static constexpr auto MAPPING_GAP = 100;

  // Bookkeeping
  MapperManager mMapperManager;
  MapperManager::Signal mDragSource, mDragDest;  // Only non-null sig if currently dragging
  MapperManager::Signal mHoverSig;               // Index of sig being hovered over
  juce::Point<int> mDragPoint;
  int mDevNameWidth = DEFAULT_DEV_NAME_WIDTH;
  int mSigWidth;  // Adjusted in resized() relative to dev name width

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListViewComponent)
};
