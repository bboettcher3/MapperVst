/*
  ==============================================================================

    ListViewComponent.cpp
    Created: 16 Oct 2021 9:46:45pm
    Author:  brady

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ListViewComponent.h"

//==============================================================================
ListViewComponent::ListViewComponent(MapperManager& manager) : mMapperManager(manager) {
  // Populate device components
  for (MapperManager::Device& device : manager.devices) {
    // First sources
    if (device.sourceSignals.size() > 0) {
      auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_OUT);
      mSourceDevices.add(newDevice);
      addAndMakeVisible(newDevice);
    }
    // Then destinations
    if (device.destSignals.size() > 0) {
      auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_IN);
      mDestDevices.add(newDevice);
      addAndMakeVisible(newDevice);
    }
  }

  manager.addListener(this);
}

ListViewComponent::~ListViewComponent() { mMapperManager.removeListener(this); }

void ListViewComponent::paint(juce::Graphics& g) {
  auto r = getLocalBounds();
  // Show number of signals
  auto dirLabelPanel = r.removeFromTop(DIR_LABEL_HEIGHT);
  g.setColour(juce::Colours::white);
  juce::String sourcesLabel = juce::String("Sources");
  g.drawText(sourcesLabel, dirLabelPanel.removeFromLeft(dirLabelPanel.getWidth() / 2),
             juce::Justification::left);
  juce::String destsLabel = juce::String("Destinations");
  g.drawText(destsLabel, dirLabelPanel, juce::Justification::right);

  if (mSourceDevices.size() == 0 && mDestDevices.size() == 0) {
    g.setColour(juce::Colours::white);
    g.drawText("No devices found", r.toFloat(), juce::Justification::centred);
  }

  /*
  // Draw dragging arrow
  if (mDragSource.sig != nullptr) {
    juce::Point<int> dragStart =
        juce::Point<int>(mDevNameWidth + mSigWidth, mDragSource.yPos).translated(0, SIG_HEIGHT / 2.0f);
    g.setColour(juce::Colours::white);
    g.drawArrow(juce::Line<int>(dragStart, mDragPoint).toFloat(), 4, 10, 10);
  }

  // Draw mappings
  for (int i = 0; i < mMapperManager.maps.size(); ++i) {
    juce::Point<int> connStart = juce::Point<int>(mDevNameWidth + mSigWidth,
  mMapperManager.maps[i].signals.first.yPos) .translated(0, SIG_HEIGHT / 2.0f); juce::Point<int> connEnd =
  juce::Point<int>(getWidth() / 2.0f + MAPPING_GAP, mMapperManager.maps[i].signals.second.yPos) .translated(0,
  SIG_HEIGHT / 2.0f); g.setColour(juce::Colours::white); g.drawArrow(juce::Line<int>(connStart,
  connEnd).toFloat(), 4, 10, 10);
  }*/
}

void ListViewComponent::resized() {
  int devWidth = (getWidth() - 2 * MAPPING_GAP) / 2.0f;
  // Source devices
  float curY = DIR_LABEL_HEIGHT;
  for (ListDeviceComponent* device : mSourceDevices) {
    device->setBounds(0, curY, devWidth, device->getHeight());
    DBG("src sigs: " + juce::String(device->getDevice().sourceSignals.size()) +
        "h: " + juce::String(device->getHeight()));
    curY += device->getHeight();
  }
  // Destination devices
  curY = DIR_LABEL_HEIGHT;
  for (ListDeviceComponent* device : mDestDevices) {
    device->setSize(devWidth, device->getHeight());
    device->setTopRightPosition(getWidth(), curY);
    DBG("dst sigs: " + juce::String(device->getDevice().destSignals.size()) +
        ", h: " + juce::String(device->getHeight()));
    curY += device->getHeight();
  }
}

void ListViewComponent::mouseDrag(const juce::MouseEvent& e) {
  // Signal mapping drag
  /* mDragDest = MapperManager::Signal();
  if (mDragSource.sig != nullptr) {
    mDragPoint = e.getPosition();
    for (MapperManager::Signal& sig : mDestSigs) {
      if (juce::Rectangle<int>((getWidth() / 2.0f) + MAPPING_GAP, sig.yPos, mSigWidth, SIG_HEIGHT)
              .contains(e.getPosition())) {
        mDragDest = sig;
      }
    }
    repaint();
  } */
}

void ListViewComponent::mouseUp(const juce::MouseEvent& e) {
  /*
  if (mDragSource.sig != nullptr && mDragDest.sig != nullptr) {
    // Add new mapping
    mpr_map newMap = mpr_map_new(1, &mDragSource.sig, 1, &mDragDest.sig);
    mpr_obj_push(newMap);
    mMapperManager.checkAddMap(newMap);
    mDragSource = MapperManager::Signal();
    mDragDest = MapperManager::Signal();
    mDragPoint = juce::Point<int>();
  }
  repaint();*/
}

void ListViewComponent::deviceAdded(MapperManager::Device& device) {
  // First sources
  if (device.sourceSignals.size() > 0) {
    auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_OUT);
    mSourceDevices.add(newDevice);
    addAndMakeVisible(newDevice);
  }
  // Then destinations
  if (device.destSignals.size() > 0) {
    auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_IN);
    mDestDevices.add(newDevice);
    addAndMakeVisible(newDevice);
  }
  resized();
  repaint();
}

void ListViewComponent::deviceModified(MapperManager::Device& device) {
  // Create or remove component depending on signals present
  // First sources
  auto iter = std::find_if(
      mSourceDevices.begin(), mSourceDevices.end(),
      [device](ListDeviceComponent* other) { return other->getDevice().dev == device.dev; });

  if (iter == mSourceDevices.end() && device.sourceSignals.size() > 0) {
    // Add new source device
    auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_OUT);
    mSourceDevices.add(newDevice);
    addAndMakeVisible(newDevice);
  } else if (iter != mSourceDevices.end() && device.sourceSignals.size() == 0) {
    // Remove empty source device
    mSourceDevices.removeObject(*iter);
  }

  // Then destinations
  iter = std::find_if(
      mDestDevices.begin(), mDestDevices.end(),
      [device](ListDeviceComponent* other) { return other->getDevice().dev == device.dev; });
  if (iter == mDestDevices.end() && device.destSignals.size() > 0) {
    // Add new dest device
    auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_IN);
    mDestDevices.add(newDevice);
    addAndMakeVisible(newDevice);
  } else if (iter != mDestDevices.end() && device.destSignals.size() == 0) {
    // Remove empty dest device
    mDestDevices.removeObject(*iter);
  }
  resized();
  repaint();
}

void ListViewComponent::deviceRemoved(MapperManager::Device& device) {
  // First sources
  if (device.sourceSignals.size() > 0) {
    for (ListDeviceComponent* deviceComp : mSourceDevices) {
      if (deviceComp->getDevice().dev == device.dev) {
        mSourceDevices.removeObject(deviceComp);
      }
    }
  }
  // Then destinations
  if (device.destSignals.size() > 0) {
    for (ListDeviceComponent* deviceComp : mDestDevices) {
      if (deviceComp->getDevice().dev == device.dev) {
        mDestDevices.removeObject(deviceComp);
      }
    }
  }
  resized();
  repaint();
}