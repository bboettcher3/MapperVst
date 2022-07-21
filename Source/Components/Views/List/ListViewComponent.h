/*
  ==============================================================================

    MapViewComponent.h
    Created: 16 Oct 2021 9:46:45pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../../MapperManager.h"
#include "ListDeviceComponent.h"

//==============================================================================
/*
 */
class ListViewComponent : public juce::Component, MapperManager::DevicesListener {
 public:
  ListViewComponent(MapperManager& manager);
  ~ListViewComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  void mouseDrag(const juce::MouseEvent& e) override;
  void mouseUp(const juce::MouseEvent& e) override;

  void deviceAdded(MapperManager::Device& device) override;
  void deviceModified(MapperManager::Device& device) override;
  void deviceRemoved(MapperManager::Device& device) override;

  MapperManager& getMapperManager() { return mMapperManager; }  // called from device callback

 private:
  static constexpr auto DIR_LABEL_HEIGHT = 20;
  static constexpr auto MAPPING_GAP = 100;


  // Bookkeeping
  MapperManager& mMapperManager;
  MapperManager::Signal mDragSource, mDragDest;  // Only non-null sig if currently dragging
  MapperManager::Signal mHoverSig;               // Index of sig being hovered over
  juce::Point<int> mDragPoint;

  // Device components
  juce::OwnedArray<ListDeviceComponent> mSourceDevices;
  juce::OwnedArray<ListDeviceComponent> mDestDevices;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListViewComponent)
};
