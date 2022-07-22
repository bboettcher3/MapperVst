/*
  ==============================================================================

    ListDeviceComponent.h
    Created: 20 Jul 2022 8:58:51pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../../MapperManager.h"
#include "ListSignalComponent.h"

//==============================================================================
/*
 */
class ListDeviceComponent : public juce::Component, MapperManager::SignalsListener {
 public:
  ListDeviceComponent(MapperManager& manager, MapperManager::Device& device, mpr_dir dir);
  ~ListDeviceComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  void mouseMove(const juce::MouseEvent& e) override;
  void mouseExit(const juce::MouseEvent& e) override;

  void signalAdded(MapperManager::Signal& signal) override;
  void signalRemoved(MapperManager::Signal& signal) override;

  MapperManager::Device& getDevice() { return mDevice; }
  juce::OwnedArray<ListSignalComponent>& getSignals() { return mSignals; }

  static constexpr int SIGNAL_HEIGHT = 30;

 private:
  // Bookkeeping
  MapperManager& mMapperManager;
  MapperManager::Device& mDevice;
  juce::String mDevName;
  mpr_dir mDir;
  bool mIsExpanded = true;
  
  // Components
  juce::OwnedArray<ListSignalComponent> mSignals;
  juce::ImageButton mBtnCollapse;
  juce::ImageButton mBtnInfo;

  void updateSize();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListDeviceComponent)
};
