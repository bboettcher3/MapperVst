/*
  ==============================================================================

    ListDeviceComponent.cpp
    Created: 20 Jul 2022 8:58:51pm
    Author:  brady

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ListDeviceComponent.h"
#include "BinaryData.h"

//==============================================================================
ListDeviceComponent::ListDeviceComponent(MapperManager::Device& device, mpr_dir dir)
    : mDevice(device), mDir(dir) {
  // Populate signal components
  std::vector<MapperManager::Signal>& signals =
      (mDir == MPR_SIG_OUT) ? device.sourceSignals : device.destSignals;
  for (MapperManager::Signal& signal : signals) {
    // TODO: populate
    auto newSignal = new ListSignalComponent(signal);
    mSignals.add(newSignal);
    addAndMakeVisible(newSignal);
  }

  // Expand/collapse button
  juce::Image arrowDown =
      juce::PNGImageFormat::loadFrom(BinaryData::arrowDownIcon_png, BinaryData::arrowDownIcon_pngSize);
  juce::Image arrowRight =
      juce::PNGImageFormat::loadFrom(BinaryData::arrowRightIcon_png, BinaryData::arrowRightIcon_pngSize);
  mBtnCollapse.setToggleable(true);
  mBtnCollapse.setToggleState(mIsExpanded, juce::dontSendNotification);
  mBtnCollapse.setClickingTogglesState(true);
  mBtnCollapse.setImages(false, false, true, arrowRight, 1.0f, juce::Colours::white, juce::Image(), 1.0f,
                         juce::Colours::lightgrey, arrowDown, 1.0f, juce::Colours::black);
  addAndMakeVisible(mBtnCollapse);

  // Info button
  juce::Image infoIcon =
      juce::PNGImageFormat::loadFrom(BinaryData::infoIcon_png, BinaryData::infoIcon_pngSize);
  mBtnInfo.setToggleable(true);
  mBtnInfo.setToggleState(mIsExpanded, juce::dontSendNotification);
  mBtnInfo.setClickingTogglesState(true);
  mBtnInfo.setImages(false, false, true, infoIcon, 1.0f, juce::Colours::white, infoIcon, 1.0f,
                     juce::Colours::lightgrey, infoIcon, 1.0f, juce::Colours::black);
  addAndMakeVisible(mBtnInfo);

  setSize(100, mIsExpanded ? signals.size() * SIGNAL_HEIGHT : SIGNAL_HEIGHT);
}

ListDeviceComponent::~ListDeviceComponent() {}

void ListDeviceComponent::paint(juce::Graphics& g) {
  g.fillAll(mDevice.colour);

  // Device name and info
  g.setColour(juce::Colours::white);
  juce::String devName = juce::String(mpr_obj_get_prop_as_str(mDevice.dev, MPR_PROP_NAME, nullptr));
  auto devNameRect = (mDir == MPR_SIG_OUT) ? getLocalBounds()
                                                 .removeFromLeft(getWidth() * DEV_ICONS_WIDTH_PERC)
                                                 .removeFromLeft(getWidth() * DEV_NAME_WIDTH_PERC)
                                           : getLocalBounds()
                                                 .removeFromRight(getWidth() * DEV_ICONS_WIDTH_PERC)
                                                 .removeFromRight(getWidth() * DEV_NAME_WIDTH_PERC);
  g.drawText(devName, getLocalBounds().removeFromLeft(getWidth() * DEV_NAME_WIDTH_PERC),
             juce::Justification::centred);

  std::vector<MapperManager::Signal>& signals =
      (mDir == MPR_SIG_OUT) ? mDevice.sourceSignals : mDevice.destSignals;
  if (mIsExpanded) {
  } else {
    juce::String collapsedMessage = juce::String(signals.size()) + juce::String(" signals");
    g.setColour(juce::Colours::white);
    g.drawText(collapsedMessage, getLocalBounds().removeFromLeft(getWidth() * DEV_NAME_WIDTH_PERC),
               juce::Justification::centred);
  }
}

void ListDeviceComponent::resized() {
  auto collapseIconRect = (mDir == MPR_SIG_OUT)
                              ? getLocalBounds().removeFromLeft(getWidth() * DEV_ICONS_WIDTH_PERC)
                              : getLocalBounds().removeFromRight(getWidth() * DEV_ICONS_WIDTH_PERC);
  mBtnCollapse.setBounds(collapseIconRect);
  auto infoIconRect = (mDir == MPR_SIG_OUT)
                          ? getLocalBounds()
                                .removeFromLeft(getWidth() * (DEV_ICONS_WIDTH_PERC + DEV_NAME_WIDTH_PERC))
                                .removeFromLeft(getWidth() * DEV_ICONS_WIDTH_PERC)
                          : getLocalBounds()
                                .removeFromRight(getWidth() * (DEV_ICONS_WIDTH_PERC + DEV_NAME_WIDTH_PERC))
                                .removeFromRight(getWidth() * DEV_ICONS_WIDTH_PERC);
  mBtnInfo.setBounds(infoIconRect);
}

void ListDeviceComponent::mouseMove(const juce::MouseEvent& e) {
  if (!mBtnInfo.isVisible()) {
    mBtnInfo.setVisible(true);
    repaint();
  }
}

void ListDeviceComponent::mouseExit(const juce::MouseEvent& e) {
  if (mBtnInfo.isVisible()) {
    mBtnInfo.setVisible(false);
    repaint();
  }
}