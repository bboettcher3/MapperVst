/*
  ==============================================================================

    MapperManager.cpp
    Created: 20 Jul 2022 7:12:43pm
    Author:  brady

  ==============================================================================
*/

#include "MapperManager.h"
#include "Utilities.h"

//==============================================================================
MapperManager::MapperManager() {
  graph = mpr_graph_new(MPR_OBJ);
  mpr_graph_add_cb(graph, deviceCallbackHandler, MPR_DEV, this);
  mpr_graph_add_cb(graph, signalCallbackHandler, MPR_SIG, this);
  mpr_graph_add_cb(graph, mapCallbackHandler, MPR_MAP, this);

  // Init signal blocks
  mpr_list devs = mpr_graph_get_list(graph, MPR_DEV);
  while (devs) {
    mpr_dev dev = *devs;
    checkAddDevice(dev);
    devs = mpr_list_get_next(devs);
  }

  // Init mappings
  mpr_list maps = mpr_graph_get_list(graph, MPR_MAP);
  while (maps) {
    mpr_map map = *maps;
    checkAddMap(map);
    maps = mpr_list_get_next(maps);
  }
}

MapperManager::~MapperManager() {
  mpr_graph_remove_cb(graph, deviceCallbackHandler, nullptr);
  mpr_graph_remove_cb(graph, signalCallbackHandler, nullptr);
  mpr_graph_remove_cb(graph, mapCallbackHandler, nullptr);
  if (graph) mpr_graph_free(graph);
}

juce::String MapperManager::Signal::getDescription(bool isDetailed) {
  juce::String sigString;
  /*int numProps = mpr_obj_get_num_props(sig, 1);
  for (int i = 0; i < numProps; ++i) {
    int propLen = 0;
    const char* propName = nullptr;
    mpr_type propType;
    const void* values = nullptr;
    mpr_prop prop =
        mpr_obj_get_prop_by_idx(sig, i, &propName, &propLen, &propType, &values, nullptr);
    DBG(juce::String(propName) + ": " + juce::String(&propType));
  } */

  sigString += juce::String(mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, nullptr));
  if (!isDetailed) return sigString;
  sigString += juce::String(" (");
  sigString += getTypeString(mpr_obj_get_prop_as_int32(sig, MPR_PROP_TYPE, nullptr));
  int sigLen = mpr_obj_get_prop_as_int32(sig, MPR_PROP_LEN, nullptr);
  if (sigLen > 1) {
    sigString += juce::String("[");
    sigString += mpr_obj_get_prop_as_int32(sig, MPR_PROP_LEN, nullptr);
    sigString += juce::String("]");
  }
  float min = mpr_obj_get_prop_as_flt(sig, MPR_PROP_MIN, nullptr);
  float max = mpr_obj_get_prop_as_flt(sig, MPR_PROP_MAX, nullptr);
  if (max > 0) {
    sigString += juce::String(" ");
    sigString += juce::String(min);
    sigString += juce::String("..");
    sigString += juce::String(max);
  }
  sigString += juce::String(")");
  return sigString;
}

void MapperManager::addListener(DevicesListener* newListener) {
  const juce::ScopedLock sl(mListenerLock);
  mDevicesListeners.addIfNotAlreadyThere(newListener);
}
void MapperManager::addListener(SignalsListener* newListener) {
  const juce::ScopedLock sl(mListenerLock);
  mSignalsListeners.addIfNotAlreadyThere(newListener);
}
void MapperManager::addListener(MapsListener* newListener) {
  const juce::ScopedLock sl(mListenerLock);
  mMapsListeners.addIfNotAlreadyThere(newListener);
}
void MapperManager::removeListener(DevicesListener* listener) {
  const juce::ScopedLock sl(mListenerLock);
  mDevicesListeners.removeFirstMatchingValue(listener);
}
void MapperManager::removeListener(SignalsListener* listener) {
  const juce::ScopedLock sl(mListenerLock);
  mSignalsListeners.removeFirstMatchingValue(listener);
}
void MapperManager::removeListener(MapsListener* listener) {
  const juce::ScopedLock sl(mListenerLock);
  mMapsListeners.removeFirstMatchingValue(listener);
}

void MapperManager::refreshGraph() {
  // Sync graph with network
  mpr_graph_poll(graph, 50);
}

void MapperManager::checkAddDevice(mpr_dev device) {
  // Skip if already added
  if (std::find_if(devices.begin(), devices.end(), [device](Device* other) { return other->dev == device; }) !=
      devices.end())
    return;

  mCurDeviceColourIdx =
      (mCurDeviceColourIdx + 1 + (Utilities::NUM_COLOURS / 2)) % Utilities::NUM_COLOURS;
  juce::Colour devColour = Utilities::getRainbow12Colour(mCurDeviceColourIdx).brighter(1.3f);
  Device* newDev = new Device(device, devColour);
  devices.add(newDev);
  DBG("add dev: " + juce::String(mpr_obj_get_prop_as_str(device, MPR_PROP_NAME, nullptr)));

  mpr_list sigs = mpr_dev_get_sigs(device, MPR_DIR_ANY);
  while (sigs) {
    mpr_sig sig = *sigs;
    checkAddSignal(sig);
    sigs = mpr_list_get_next(sigs);
  }

  juce::ScopedLock lock(mListenerLock);
  for (DevicesListener* listener : mDevicesListeners) {
    listener->deviceAdded(newDev);
  }
}

void MapperManager::removeDevice(mpr_dev dev) {
  auto deviceIter =
      std::find_if(devices.begin(), devices.end(), [dev](Device* other) { return other->dev == dev; });
  if (deviceIter == devices.end()) return;  // Skip if device not found
  
  // Must call listeners before destroying reference to device
  juce::ScopedLock lock(mListenerLock);
  for (DevicesListener* listener : mDevicesListeners) {
    listener->deviceRemoved(*deviceIter);
  }

  DBG("rem dev: " + juce::String(mpr_obj_get_prop_as_str(dev, MPR_PROP_NAME, nullptr)));

  devices.removeObject(*deviceIter);
}

MapperManager::Signal* MapperManager::checkAddSignal(mpr_sig sig) {
  Device* device = getDevice(sig);

  bool isSource = mpr_obj_get_prop_as_int32(sig, MPR_PROP_DIR, nullptr) == MPR_DIR_OUT;
  juce::OwnedArray<Signal>& signals = isSource ? device->sourceSignals : device->destSignals;
  // Skip if already added
  auto iter = std::find_if(signals.begin(), signals.end(), [sig](Signal* other) { return other->sig == sig; });
  if (iter != signals.end()) return *iter;
  // Add signal to its list
  Signal* newSig = new Signal(sig);
  signals.add(newSig);
  DBG("add sig: " + juce::String(mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, nullptr)));

  // Call signal listeners if device matches
  juce::ScopedLock lock(mListenerLock);
  for (SignalsListener* listener : mSignalsListeners) {
    if (listener->device->dev == device->dev) {
      listener->signalAdded(newSig);
    }
  }
  // Call devices listener for new signal
  for (DevicesListener* listener : mDevicesListeners) {
    listener->deviceModified(device);
  }

  return newSig;
}

void MapperManager::removeSignal(mpr_sig sig) {
  Device* device = getDevice(sig);
  bool isSource = mpr_obj_get_prop_as_int32(sig, MPR_PROP_DIR, nullptr) == MPR_DIR_OUT;
  juce::OwnedArray<Signal>& signals = isSource ? device->sourceSignals : device->destSignals;
  
  auto iter = std::find_if(signals.begin(), signals.end(), [sig](Signal* other) { return other->sig == sig; });
  if (iter == signals.end()) return; // Skip if not found

  // Call signal listeners if device matches before erasing signal
  juce::ScopedLock lock(mListenerLock);
  for (SignalsListener* listener : mSignalsListeners) {
    if (listener->device->dev == device->dev) {
      listener->signalRemoved(*iter);
    }
  }

  DBG("rem sig: " + juce::String(mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, nullptr)));

  signals.removeObject(*iter);

  // Call devices listener for removed signal
  for (DevicesListener* listener : mDevicesListeners) {
    listener->deviceModified(device);
  }
}

MapperManager::Map* MapperManager::checkAddMap(mpr_map map) {
  mpr_list sourceSigs = mpr_map_get_sigs(map, MPR_LOC_SRC);
  std::vector<Signal*> sourceSignals;
  for (int i = 0; i < mpr_list_get_size(sourceSigs); ++i) {
    Signal* sourceSignal = checkAddSignal(mpr_list_get_idx(sourceSigs, i));
    sourceSignals.push_back(sourceSignal);
  }
  mpr_list destSigs = mpr_map_get_sigs(map, MPR_LOC_DST);
  std::vector<Signal*> destSignals;
  for (int i = 0; i < mpr_list_get_size(destSigs); ++i) {
    Signal* destSignal = checkAddSignal(mpr_list_get_idx(destSigs, i));
    destSignals.push_back(destSignal);
  }
  
  // Skip if already added
  auto mapIter = std::find_if(maps.begin(), maps.end(), [map](Map* other) { return other->map == map; });
  if (mapIter != maps.end()) {
    return *mapIter;
  }
  
  Map* newMap = new Map(map, sourceSignals, destSignals);
  maps.add(newMap);

  if (newMap->sourceSignals.size() > 0 && newMap->destSignals.size() > 0) {
    DBG("add map: " +
        juce::String(
            mpr_obj_get_prop_as_str(newMap->sourceSignals.front()->sig, MPR_PROP_NAME, nullptr)) +
        juce::String(" -> ") +
        juce::String(mpr_obj_get_prop_as_str(newMap->destSignals.front()->sig, MPR_PROP_NAME, nullptr)));
  }

  // Call map added listeners
  juce::ScopedLock lock(mListenerLock);
  for (MapsListener* listener : mMapsListeners) {
    listener->mapAdded(newMap);
  }

  return newMap;
}

void MapperManager::removeMap(mpr_map map) {
  auto iter = std::find_if(maps.begin(), maps.end(), [map](Map* other) { return other->map == map; });
  if (iter != maps.end()) {
    MapperManager::Map* remMap = *iter;
    if (remMap->sourceSignals.size() > 0 && remMap->destSignals.size() > 0) {
      DBG("rem map: " +
          juce::String(
              mpr_obj_get_prop_as_str(remMap->sourceSignals.front()->sig, MPR_PROP_NAME, nullptr)) +
          juce::String(" -> ") +
          juce::String(
              mpr_obj_get_prop_as_str(remMap->destSignals.front()->sig, MPR_PROP_NAME, nullptr)));
    }


    // Call map removed listeners
    juce::ScopedLock lock(mListenerLock);
    for (MapsListener* listener : mMapsListeners) {
      listener->mapRemoved(remMap);
    }
    mpr_map_release(remMap->map);
    maps.removeObject(remMap);
  }
}

void MapperManager::setSelectedMap(Map* map) {
  if (map != mSelectedMap) {
    juce::ScopedLock lock(mListenerLock);
    for (MapsListener* listener : mMapsListeners) {
      listener->mapSelected(map);
    }
  }
  mSelectedMap = map;
}

MapperManager::Device* MapperManager::getDevice(mpr_sig sig) {
  mpr_dev dev = mpr_sig_get_dev(sig);
  auto devIter =
      std::find_if(devices.begin(), devices.end(), [dev](Device* other) { return other->dev == dev; });
  jassert(devIter != devices.end());
  return *devIter;
}

void MapperManager::deviceCallbackHandler(mpr_graph g, mpr_dev dev, mpr_graph_evt e, const void* context) {
  if (context == nullptr) return;
  MapperManager* component = (MapperManager*)context;
  switch (e) {
    case MPR_OBJ_NEW: {
      component->checkAddDevice(dev);
      break;
    }
    case MPR_OBJ_REM:
    case MPR_OBJ_EXP: {
      component->removeDevice(dev);
      break;
    }
    case MPR_OBJ_MOD: {
      // TODO
      break;
    }
    default: {
    }
  }
}

void MapperManager::signalCallbackHandler(mpr_graph g, mpr_sig sig, mpr_graph_evt e, const void* context) {
  if (context == nullptr) return;
  MapperManager* component = (MapperManager*)context;
  switch (e) {
    case MPR_OBJ_NEW: {
      component->checkAddSignal(sig);
      break;
    }
    case MPR_OBJ_REM:
    case MPR_OBJ_EXP: {
      component->removeSignal(sig);
      break;
    }
    case MPR_OBJ_MOD: {
      // TODO
      break;
    }
    default: {
    }
  }
}

void MapperManager::mapCallbackHandler(mpr_graph g, mpr_map map, mpr_graph_evt e, const void* context) {
  if (context == nullptr) return;
  MapperManager* component = (MapperManager*)context;
  switch (e) {
    case MPR_OBJ_NEW: {
      component->checkAddMap(map);
      break;
    }
    case MPR_OBJ_REM:
    case MPR_OBJ_EXP: {
      component->removeMap(map);
      break;
    }
    case MPR_OBJ_MOD: {
      // TODO
      break;
    }
    default: {
    }
  }
}

juce::String MapperManager::getTypeString(int type) {
  switch (type) {
    case MPR_BOOL:
      return juce::String("bool");
    case MPR_INT32:
      return juce::String("int");
    case MPR_FLT:
      return juce::String("float");
    case MPR_STR:
      return juce::String("string");
    default:
      return juce::String("");
  }
}