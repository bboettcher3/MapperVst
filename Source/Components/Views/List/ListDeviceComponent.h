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
class ListDeviceComponent : public juce::Component {
 public:
  ListDeviceComponent(MapperManager::Device& device, mpr_dir dir);
  ~ListDeviceComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  void mouseMove(const juce::MouseEvent& e) override;
  void mouseExit(const juce::MouseEvent& e) override;

 private:
  static constexpr int SIGNAL_HEIGHT = 30;
  static constexpr float DEV_NAME_WIDTH_PERC = 0.3f;
  static constexpr float DEV_ICONS_WIDTH_PERC = 0.1f;

  MapperManager::Device& mDevice;
  mpr_dir mDir;

  // Bookkeeping
  bool mIsExpanded = false;
  
  // Components
  juce::OwnedArray<ListSignalComponent> mSignals;
  juce::ImageButton mBtnCollapse;
  juce::ImageButton mBtnInfo;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListDeviceComponent)
};
