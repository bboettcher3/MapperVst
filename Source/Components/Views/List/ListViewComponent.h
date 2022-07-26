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
class ListViewComponent : public juce::Component,
                          juce::KeyListener,
                          MapperManager::DevicesListener,
                          MapperManager::MapsListener {
 public:
  ListViewComponent(MapperManager& manager);
  ~ListViewComponent() override;

  void paint(juce::Graphics&) override;
  void paintOverChildren(juce::Graphics&) override;
  void resized() override;

  bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;

  void mouseDrag(const juce::MouseEvent& e) override;
  void mouseUp(const juce::MouseEvent& e) override;
  void mouseMove(const juce::MouseEvent& e) override;

  void deviceAdded(MapperManager::Device* device) override;
  void deviceModified(MapperManager::Device* device) override;
  void deviceRemoved(MapperManager::Device* device) override;

  void mapAdded(MapperManager::Map* map) override;
  void mapSelected(MapperManager::Map* map) override;
  void mapModified(MapperManager::Map* map) override;
  void mapRemoved(MapperManager::Map* map) override;

  MapperManager& getMapperManager() { return mMapperManager; }  // called from device callback

 private:
  static constexpr int DIR_LABEL_HEIGHT = 20;
  static constexpr int MAPPING_GAP = 100;
  static constexpr int MIN_MAP_SELECT_DISTANCE = 10;

  typedef struct ListMap {
    ListMap(MapperManager::Map* map, std::vector<ListSignalComponent*> sourceSigComps,
            std::vector<ListSignalComponent*> destSigComps)
        : map(map), sourceSigComps(sourceSigComps), destSigComps(destSigComps) {}
    MapperManager::Map* map;
    juce::Path path;
    std::vector<ListSignalComponent*> sourceSigComps;
    std::vector<ListSignalComponent*> destSigComps;
  } ListMap;

  // Bookkeeping
  MapperManager& mMapperManager;
  ListSignalComponent* mDragSource = nullptr; // Only non-null sig if currently dragging
  ListSignalComponent* mDragDestSig = nullptr;
  ListMap* mDragDestMap = nullptr;
  ListSignalComponent* mHoverSig = nullptr; // Only non-null if hovering over a signal
  ListMap* mHoverMap = nullptr;
  ListMap* mSelectedMap = nullptr;
  juce::Point<int> mDragPoint;
  juce::Path mDragPath;
  std::vector<ListMap> mListMaps;
  int mDevWidth;

  // Device components
  juce::OwnedArray<ListDeviceComponent> mSourceDevices;
  juce::OwnedArray<ListDeviceComponent> mDestDevices;

  void addListMap(MapperManager::Map* map);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListViewComponent)
};
