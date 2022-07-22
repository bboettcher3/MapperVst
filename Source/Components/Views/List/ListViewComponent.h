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
  void paintOverChildren(juce::Graphics&) override;
  void resized() override;

  void mouseDrag(const juce::MouseEvent& e) override;
  void mouseUp(const juce::MouseEvent& e) override;

  void deviceAdded(MapperManager::Device* device) override;
  void deviceModified(MapperManager::Device* device) override;
  void deviceRemoved(MapperManager::Device* device) override;

  MapperManager& getMapperManager() { return mMapperManager; }  // called from device callback

 private:
  static constexpr auto DIR_LABEL_HEIGHT = 20;
  static constexpr auto MAPPING_GAP = 100;

  typedef struct ListMap {
    ListMap(MapperManager::Map* map, ListSignalComponent* sourceSignal,
            ListSignalComponent* destSignal)
        : map(map), sourceSignal(sourceSignal), destSignal(destSignal) {}
    MapperManager::Map* map;
    ListSignalComponent* sourceSignal;  // TODO: vector to allow complex
    ListSignalComponent* destSignal;
  } ListMap;

  // Bookkeeping
  MapperManager& mMapperManager;
  ListSignalComponent* mDragSource = nullptr; // Only non-null sig if currently dragging
  ListSignalComponent* mDragDest = nullptr;
  ListSignalComponent* mHoverSig = nullptr; // Only non-null if hovering over a signal
  juce::Point<int> mDragPoint;
  std::vector<ListMap> mListMaps;
  int mDevWidth;

  // Device components
  juce::OwnedArray<ListDeviceComponent> mSourceDevices;
  juce::OwnedArray<ListDeviceComponent> mDestDevices;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListViewComponent)
};
