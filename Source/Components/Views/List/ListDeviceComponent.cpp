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
ListDeviceComponent::ListDeviceComponent(MapperManager& manager, MapperManager::Device* device,
                                         mpr_dir dir)
    : mMapperManager(manager), mDevice(device), mDir(dir), MapperManager::SignalsListener(device) {
  mDevName = juce::String(mpr_obj_get_prop_as_str(mDevice->dev, MPR_PROP_NAME, nullptr));

  // Populate signal components
  juce::OwnedArray<MapperManager::Signal>& signals =
      (mDir == MPR_DIR_OUT) ? device->sourceSignals : device->destSignals;
  for (MapperManager::Signal* signal : signals) {
    juce::Colour sigColour =
        mDevice->colour.darker((mSignals.size() % 2) ? SIG_EVEN_DARKER : SIG_ODD_DARKER);
    auto newSignal = new ListSignalComponent(signal, sigColour);
    mSignals.add(newSignal);
    addAndMakeVisible(newSignal);
  }

  // Expand/collapse button
  juce::Image arrowExpanded = juce::PNGImageFormat::loadFrom(BinaryData::arrowDownIcon_png,
                                                             BinaryData::arrowDownIcon_pngSize);
  juce::Image arrowCollapsed =
      (mDir == MPR_DIR_OUT) ? juce::PNGImageFormat::loadFrom(BinaryData::arrowRightIcon_png,
                                                             BinaryData::arrowRightIcon_pngSize)
                            : juce::PNGImageFormat::loadFrom(BinaryData::arrowLeftIcon_png,
                                                             BinaryData::arrowLeftIcon_pngSize);
  mBtnCollapse.setToggleable(true);
  mBtnCollapse.setToggleState(mIsExpanded, juce::dontSendNotification);
  mBtnCollapse.setClickingTogglesState(true);
  mBtnCollapse.setImages(false, true, true, arrowCollapsed, 1.0f, juce::Colours::black,
                         juce::Image(), 1.0f, juce::Colours::lightgrey, arrowExpanded, 1.0f,
                         juce::Colours::black);
  mBtnCollapse.onClick = [this] {
    mIsExpanded = !mIsExpanded;
    updateSize();
  };
  addAndMakeVisible(mBtnCollapse);

  // Info button
  juce::Image infoIcon =
      juce::PNGImageFormat::loadFrom(BinaryData::infoIcon_png, BinaryData::infoIcon_pngSize);
  mBtnInfo.setImages(false, true, true, infoIcon, 1.0f, juce::Colours::black, infoIcon, 1.0f,
                     juce::Colours::lightgrey, infoIcon, 1.0f, juce::Colours::white);
  mBtnInfo.setToggleable(true);
  mBtnInfo.setClickingTogglesState(true);
  mBtnInfo.onStateChange = [this] {
    for (ListSignalComponent* sigComp : mSignals) {
      sigComp->setShowDetails(mBtnInfo.getToggleState());
    }
  };
  addChildComponent(mBtnInfo);

  mMapperManager.addListener(this);

  addMouseListener(this, true);

  updateSize();
}

ListDeviceComponent::~ListDeviceComponent() { mMapperManager.removeListener(this); }

void ListDeviceComponent::paint(juce::Graphics& g) {
  auto devPanel =
      (mDir == MPR_DIR_OUT)
          ? juce::Rectangle<int>(0, 0, getWidth() / 2.0f, SIGNAL_HEIGHT)
          : juce::Rectangle<int>(getWidth() / 2.0f, 0, getWidth() / 2.0f, SIGNAL_HEIGHT);

  g.setColour(mDevice->colour);
  g.fillRect(devPanel.withSizeKeepingCentre(devPanel.getWidth() - 2, devPanel.getHeight() - 2));

  g.setColour(juce::Colours::black);
  g.drawRect(devPanel, 1);  // draw an outline around the component

  // Device name and info
  g.setColour(juce::Colours::black);
  g.drawText(mDevName, devPanel, juce::Justification::centred);

  juce::OwnedArray<MapperManager::Signal>& signals =
      (mDir == MPR_DIR_OUT) ? mDevice->sourceSignals : mDevice->destSignals;
  if (!mIsExpanded) {
    auto collapsedArea = (mDir == MPR_DIR_OUT) ? devPanel.translated(getWidth() / 2.0f, 0)
                                               : devPanel.translated(-getWidth() / 2.0f, 0);
    g.setColour(mDevice->colour);
    g.fillRect(collapsedArea);

    juce::String collapsedMessage = juce::String((int)signals.size()) + juce::String(" signals");
    g.setColour(juce::Colours::black);
    g.drawText(collapsedMessage, collapsedArea, juce::Justification::centred);
  }
}

void ListDeviceComponent::resized() {
  auto devPanel =
      (mDir == MPR_DIR_OUT)
          ? getLocalBounds().withSize(getWidth() / 2.0f, SIGNAL_HEIGHT)
          : getLocalBounds().removeFromTop(SIGNAL_HEIGHT).removeFromRight(getWidth() / 2.0f);
  // Expand/collapse arrow
  auto collapseIconRect = (mDir == MPR_DIR_OUT) ? devPanel.removeFromLeft(SIGNAL_HEIGHT)
                                                : devPanel.removeFromRight(SIGNAL_HEIGHT);
  mBtnCollapse.setBounds(
      collapseIconRect.withSizeKeepingCentre(SIGNAL_HEIGHT / 2, SIGNAL_HEIGHT / 2));

  // Info button
  auto infoIconRect = (mDir == MPR_DIR_OUT) ? devPanel.removeFromRight(SIGNAL_HEIGHT)
                                            : devPanel.removeFromLeft(SIGNAL_HEIGHT);
  mBtnInfo.setBounds(infoIconRect.withSizeKeepingCentre(SIGNAL_HEIGHT / 2, SIGNAL_HEIGHT / 2));

  auto sigArea = (mDir == MPR_DIR_OUT) ? getLocalBounds().removeFromRight(getWidth() / 2.0f)
                                       : getLocalBounds().removeFromLeft(getWidth() / 2.0f);
  if (mIsExpanded) {
    for (ListSignalComponent* signal : mSignals) {
      signal->setVisible(true);
      signal->setBounds(sigArea.removeFromTop(SIGNAL_HEIGHT));
    }
  } else {
    for (ListSignalComponent* signal : mSignals) {
      signal->setVisible(false);
      signal->setBounds(sigArea);
    }
  }
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

void ListDeviceComponent::signalAdded(MapperManager::Signal* signal) {
  auto iter = std::find_if(mSignals.begin(), mSignals.end(), [signal](ListSignalComponent* other) {
    return other->getSignal()->sig == signal->sig;
  });
  if (iter == mSignals.end()) {
    juce::Colour sigColour =
        mDevice->colour.darker((mSignals.size() % 2) ? SIG_EVEN_DARKER : SIG_ODD_DARKER);
    auto newSignal = new ListSignalComponent(signal, sigColour);
    mSignals.add(newSignal);
    addAndMakeVisible(newSignal);
    updateSize();
  }
}
void ListDeviceComponent::signalRemoved(MapperManager::Signal* signal) {
  for (ListSignalComponent* signalComp : mSignals) {
    if (signalComp->getSignal()->sig == signal->sig) {
      mSignals.removeObject(signalComp);
      // Update signal colours
      for (int i = 0; i < mSignals.size(); ++i) {
        juce::Colour sigColour =
            mDevice->colour.darker((i % 2) ? SIG_EVEN_DARKER : SIG_ODD_DARKER);
        mSignals[i]->setColour(sigColour);
      }
      updateSize();
      break;
    }
  }
}

void ListDeviceComponent::updateSize() {
  float height =
      mIsExpanded ? juce::jmax(SIGNAL_HEIGHT, (int)mSignals.size() * SIGNAL_HEIGHT) : SIGNAL_HEIGHT;
  setSize(100, height);
  juce::Component* parent = getParentComponent();
  if (parent != nullptr) {
    parent->repaint();
    parent->resized();
  }
  resized();
  repaint();
}