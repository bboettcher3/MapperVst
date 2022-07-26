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
  for (MapperManager::Device* device : manager.devices) {
    // First sources
    if (device->sourceSignals.size() > 0) {
      auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_OUT);
      mSourceDevices.add(newDevice);
      addAndMakeVisible(newDevice);
    }
    // Then destinations
    if (device->destSignals.size() > 0) {
      auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_IN);
      mDestDevices.add(newDevice);
      addAndMakeVisible(newDevice);
    }
  }

  // Populate maps
  for (MapperManager::Map* map : manager.maps) {
    addListMap(map);
  }

  manager.addListener((MapperManager::DevicesListener*)this);
  manager.addListener((MapperManager::MapsListener*)this);
  addMouseListener(this, true);
  addKeyListener(this);
}

ListViewComponent::~ListViewComponent() {
  mMapperManager.removeListener((MapperManager::DevicesListener*)this);
  mMapperManager.removeListener((MapperManager::MapsListener*)this);
}

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
}

void ListViewComponent::paintOverChildren(juce::Graphics& g) {
  juce::AffineTransform shadowTransform = juce::AffineTransform::translation(0.0f, 3.0f);

  // Draw mappings
  for (ListMap& listMap : mListMaps) {
    juce::Colour mapColour =
        (mSelectedMap == &listMap)
            ? juce::Colours::palevioletred
            : ((mHoverMap == &listMap) ? juce::Colours::palegreen : juce::Colours::white);
    juce::Path mapPath;
    for (ListSignalComponent* sourceSigComp : listMap.sourceSigComps) {
      for (ListSignalComponent* destSigComp : listMap.destSigComps) {
        int sourceY = getLocalPoint(sourceSigComp, juce::Point<int>(0, 0)).getY() +
                      ListDeviceComponent::SIGNAL_HEIGHT / 2.0f;
        juce::Point<int> dragStart = juce::Point<int>(mDevWidth, sourceY);
        int destY = getLocalPoint(destSigComp, juce::Point<int>(0, 0)).getY() +
                    ListDeviceComponent::SIGNAL_HEIGHT / 2.0f;
        juce::Point<int> connStart = juce::Point<int>(mDevWidth, sourceY);

        // Draw bezier curve connecting maps
        mapPath.startNewSubPath(connStart.toFloat());
        mapPath.cubicTo(getWidth() / 2.0f, sourceY, getWidth() / 2.0f, destY,
                        getWidth() - mDevWidth, destY);
      }
    }
    // Shadow for path
    g.setColour(mapColour.darker());
    g.strokePath(mapPath, juce::PathStrokeType(2), shadowTransform);

    g.setColour(mapColour);
    g.strokePath(mapPath, juce::PathStrokeType(4));
    listMap.path = mapPath;
  }

  // Draw dragging path with bezier curve
  if (mDragSource != nullptr) {
    int sourceY = getLocalPoint(mDragSource, mDragSource->getLocalBounds().getTopLeft()).getY() +
                  ListDeviceComponent::SIGNAL_HEIGHT / 2.0f;
    juce::Point<int> dragStart = juce::Point<int>(mDevWidth, sourceY);
    g.setColour(juce::Colours::white);
    juce::Path dragPath;
    float ctrlCenter = dragStart.getX() + (mDragPoint.getX() - dragStart.getX()) / 2.0f;
    dragPath.startNewSubPath(dragStart.toFloat());
    dragPath.cubicTo(ctrlCenter, sourceY, ctrlCenter, mDragPoint.getY(), mDragPoint.getX(),
                     mDragPoint.getY());
    mDragPath = dragPath;
    g.strokePath(dragPath, juce::PathStrokeType(4));
    // Shadow for path
    g.setColour(juce::Colours::lightgrey);
    g.strokePath(dragPath, juce::PathStrokeType(2), shadowTransform);
  }
}

void ListViewComponent::resized() {
  mDevWidth = (getWidth() - 2 * MAPPING_GAP) / 2.0f;
  // Source devices
  float curY = DIR_LABEL_HEIGHT;
  for (ListDeviceComponent* device : mSourceDevices) {
    device->setBounds(0, curY, mDevWidth, device->getHeight());
    curY += device->getHeight();
  }
  // Destination devices
  curY = DIR_LABEL_HEIGHT;
  for (ListDeviceComponent* device : mDestDevices) {
    device->setSize(mDevWidth, device->getHeight());
    device->setTopRightPosition(getWidth(), curY);
    curY += device->getHeight();
  }
}

bool ListViewComponent::keyPressed(const juce::KeyPress& key,
                                   juce::Component* originatingComponent) {
  if (key.isKeyCode(juce::KeyPress::deleteKey) && mSelectedMap != nullptr) {
    mMapperManager.removeMap(mSelectedMap->map->map);
    return true;
  }
  return false;
}


void ListViewComponent::mouseDrag(const juce::MouseEvent& e) {
  // Signal mapping drag

  // Check for destinations if we have a source
  if (mDragSource == nullptr) {
    mDragDest = nullptr;
    // Check for a source
    for (ListDeviceComponent* devComp : mSourceDevices) {
      for (ListSignalComponent* sigComp : devComp->getSignals()) {
        if (e.eventComponent == sigComp) {
          mDragSource = sigComp;
          mDragPoint = e.getEventRelativeTo(this).getPosition();
        }
      }
    }
  } else {
    mDragDest = nullptr; // Reset every frame in case mouse dragged outside
    auto thisE = e.getEventRelativeTo(this);
    mDragPoint = thisE.getPosition();
    for (ListDeviceComponent* devComp : mDestDevices) {
      for (ListSignalComponent* sigComp : devComp->getSignals()) {
        bool isInside = sigComp->contains(sigComp->getLocalPoint(this, thisE.getPosition()));
        if (isInside) {
          mDragDest = sigComp;
        }
      }
    }
  }

  mouseMove(e.getEventRelativeTo(this));

  repaint();
}

void ListViewComponent::mouseUp(const juce::MouseEvent& e) {
  // Check if a new mapping should be created
  if (mDragSource != nullptr && mDragDest != nullptr) {
    // Add new mapping
    mpr_map newMap =
        mpr_map_new(1, &mDragSource->getSignal()->sig, 1, &mDragDest->getSignal()->sig);
    mpr_obj_set_prop(newMap, MPR_PROP_EXPR, nullptr, 1, MPR_STR,
                     MapperManager::DEFAULT_MAP_EXPRESSION, 1);
    mpr_obj_push(newMap);
    MapperManager::Map* map = mMapperManager.checkAddMap(newMap);
    auto iter = std::find_if(mListMaps.begin(), mListMaps.end(),
                             [map](ListMap other) { return other.map->map == map->map; });
    if (iter != mListMaps.end()) {
      mSelectedMap = &(*iter);
      mMapperManager.setSelectedMap(map);
    }
    
  } else {
    mSelectedMap = mHoverMap;
    mMapperManager.setSelectedMap((mSelectedMap != nullptr) ? mSelectedMap->map : nullptr);
  }
  mDragPoint = juce::Point<int>();
  mDragSource = nullptr;
  mDragDest = nullptr;

  repaint();
}

void ListViewComponent::mouseMove(const juce::MouseEvent& e) {
  // TODO: a bit messy being called from mouseDrag(), can maybe clean up
  // Check for hovering inside signal components
  auto thisE = e.getEventRelativeTo(this);
  for (ListDeviceComponent* devComp : mSourceDevices) {
    for (ListSignalComponent* sigComp : devComp->getSignals()) {
      bool isInside = sigComp->contains(sigComp->getLocalPoint(this, thisE.getPosition()));
      sigComp->setIsHovering(isInside || mDragSource == sigComp);
    }
  }
  for (ListDeviceComponent* devComp : mDestDevices) {
    for (ListSignalComponent* sigComp : devComp->getSignals()) {
      bool isInside = sigComp->contains(sigComp->getLocalPoint(this, thisE.getPosition()));
      sigComp->setIsHovering(isInside || mDragDest == sigComp);
    }
  }
  
  // Check for hovering over map if not dragging
  mHoverMap = nullptr;
  if (mDragSource == nullptr) {
    for (ListMap& listMap : mListMaps) {
      juce::Point<float> pathPoint;
      listMap.path.getNearestPoint(thisE.getPosition().toFloat(), pathPoint);
      float distance = thisE.getPosition().toFloat().getDistanceFrom(pathPoint);
      if (distance < MIN_MAP_SELECT_DISTANCE) {
        mHoverMap = &listMap;
      }
    }
  }

  repaint();
}

void ListViewComponent::deviceAdded(MapperManager::Device* device) {
  // First sources
  if (device->sourceSignals.size() > 0) {
    auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_OUT);
    mSourceDevices.add(newDevice);
    addAndMakeVisible(newDevice);
  }
  // Then destinations
  if (device->destSignals.size() > 0) {
    auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_IN);
    mDestDevices.add(newDevice);
    addAndMakeVisible(newDevice);
  }
  resized();
  repaint();
}

void ListViewComponent::deviceModified(MapperManager::Device* device) {
  // Create or remove component depending on signals present
  // First sources
  auto iter = std::find_if(
      mSourceDevices.begin(), mSourceDevices.end(),
      [device](ListDeviceComponent* other) { return other->getDevice()->dev == device->dev; });

  if (iter == mSourceDevices.end() && device->sourceSignals.size() > 0) {
    // Add new source device
    auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_OUT);
    mSourceDevices.add(newDevice);
    addAndMakeVisible(newDevice);
  } else if (iter != mSourceDevices.end() && device->sourceSignals.size() == 0) {
    // Remove empty source device
    mSourceDevices.removeObject(*iter);
  }

  // Then destinations
  iter = std::find_if(
      mDestDevices.begin(), mDestDevices.end(),
      [device](ListDeviceComponent* other) { return other->getDevice()->dev == device->dev; });
  if (iter == mDestDevices.end() && device->destSignals.size() > 0) {
    // Add new dest device
    auto newDevice = new ListDeviceComponent(mMapperManager, device, MPR_DIR_IN);
    mDestDevices.add(newDevice);
    addAndMakeVisible(newDevice);
  } else if (iter != mDestDevices.end() && device->destSignals.size() == 0) {
    // Remove empty dest device
    mDestDevices.removeObject(*iter);
  }
  resized();
  repaint();
}

void ListViewComponent::deviceRemoved(MapperManager::Device* device) {
  // First sources
  if (device->sourceSignals.size() > 0) {
    for (ListDeviceComponent* deviceComp : mSourceDevices) {
      if (deviceComp->getDevice()->dev == device->dev) {
        mSourceDevices.removeObject(deviceComp);
      }
    }
  }
  // Then destinations
  if (device->destSignals.size() > 0) {
    for (ListDeviceComponent* deviceComp : mDestDevices) {
      if (deviceComp->getDevice()->dev == device->dev) {
        mDestDevices.removeObject(deviceComp);
      }
    }
  }
  resized();
  repaint();
}

void ListViewComponent::mapAdded(MapperManager::Map* map) {
  auto iter = std::find_if(
      mListMaps.begin(), mListMaps.end(), [map](ListMap& other) { return (other.map->map == map); });
  if (iter != mListMaps.end()) return; // Skip if already added
  addListMap(map);
  resized();
  repaint();
}

void ListViewComponent::mapSelected(MapperManager::Map* map) {
  if (map == nullptr) {
    mSelectedMap = nullptr;
    return;
  }
  auto iter = std::find_if(mListMaps.begin(), mListMaps.end(),
                           [map](ListMap& other) { return other.map->map == map->map; });
  if (iter != mListMaps.end()) {
    mSelectedMap = &(*iter);
  }
}

void ListViewComponent::mapModified(MapperManager::Map* map) {
  // TODO
  resized();
  repaint();
}

void ListViewComponent::mapRemoved(MapperManager::Map* map) {
  mHoverMap = nullptr;
  mListMaps.erase(std::remove_if(mListMaps.begin(), mListMaps.end(), [map](ListMap other) {
    return other.map->map == map->map; }), mListMaps.end());
  resized();
  repaint();
}

void ListViewComponent::addListMap(MapperManager::Map* map) {
  std::vector<ListSignalComponent*> sourceSigComps;
  std::vector<ListSignalComponent*> destSigComps;

  // Find source signal component (TODO: clean this triple loop shit up)
  for (MapperManager::Signal* sig : map->sourceSignals) {
    bool sigFound = false;
    for (ListDeviceComponent* devComp : mSourceDevices) {
      for (ListSignalComponent* sigComp : devComp->getSignals()) {
        if (sig == sigComp->getSignal()) {
          sourceSigComps.push_back(sigComp);
          sigFound = true;
          break;
        }
      }
      if (sigFound) break;
    }
  }
  // Find dest signal component
  for (MapperManager::Signal* sig : map->destSignals) {
    bool sigFound = false;
    for (ListDeviceComponent* devComp : mDestDevices) {
      for (ListSignalComponent* sigComp : devComp->getSignals()) {
        if (sig == sigComp->getSignal()) {
          destSigComps.push_back(sigComp);
          sigFound = true;
          break;
        }
      }
      if (sigFound) break;
    }
  }
  mListMaps.push_back(ListMap(map, sourceSigComps, destSigComps));
}