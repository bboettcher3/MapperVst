/*
  ==============================================================================

    MapperManager.cpp
    Created: 20 Jul 2022 7:12:43pm
    Author:  brady

  ==============================================================================
*/

#include "MapperManager.h"

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

void MapperManager::addListener(DevicesListener* newListener) {
  mDevicesListeners.addIfNotAlreadyThere(newListener);
}
void MapperManager::addListener(SignalsListener* newListener) {
  mSignalsListeners.addIfNotAlreadyThere(newListener);
}
void MapperManager::removeListener(DevicesListener* listener) {
  mDevicesListeners.removeFirstMatchingValue(listener);
}
void MapperManager::removeListener(SignalsListener* listener) {
  mSignalsListeners.removeFirstMatchingValue(listener);
}

void MapperManager::refreshGraph() {
  // Sync graph with network
  mpr_graph_poll(graph, 500);
}

void MapperManager::checkAddDevice(mpr_dev device) {
  // Skip if already added
  if (std::find_if(devices.begin(), devices.end(), [device](Device other) { return other.dev == device; }) !=
      devices.end())
    return;

  /* TODO: device colors */
  devices.push_back(Device(device, juce::Colours::palegreen));
  DBG("add dev: " + juce::String(mpr_obj_get_prop_as_str(device, MPR_PROP_NAME, nullptr)));

  mpr_list sigs = mpr_dev_get_sigs(device, MPR_DIR_ANY);
  while (sigs) {
    mpr_sig sig = *sigs;
    checkAddSignal(sig);
    sigs = mpr_list_get_next(sigs);
  }

  for (DevicesListener* listener : mDevicesListeners) {
    listener->deviceAdded(devices.back());
  }
}

void MapperManager::removeDevice(mpr_dev dev) {
  auto deviceIter =
      std::find_if(devices.begin(), devices.end(), [dev](Device other) { return other.dev == dev; });
  if (deviceIter == devices.end()) return;  // Skip if device not found
  // Must call listeners before destroying reference to device
  for (DevicesListener* listener : mDevicesListeners) {
    listener->deviceRemoved(*deviceIter);
  }

  DBG("rem dev: " + juce::String(mpr_obj_get_prop_as_str(dev, MPR_PROP_NAME, nullptr)));


  devices.erase(
      std::remove_if(devices.begin(), devices.end(), [dev](Device other) { return other.dev == dev; }),
      devices.end());
}

MapperManager::Signal& MapperManager::checkAddSignal(mpr_sig sig) {
  Device& device = getDevice(sig);

  bool isSource = mpr_obj_get_prop_as_int32(sig, MPR_PROP_DIR, nullptr) == MPR_DIR_OUT;
  std::vector<Signal>& signals = isSource ? device.sourceSignals : device.destSignals;
  // Skip if already added
  auto iter = std::find_if(signals.begin(), signals.end(), [sig](Signal other) { return other.sig == sig; });
  if (iter != signals.end()) return *iter;
  // Add signal to its list
  juce::Colour baseColour = juce::Colour(BASE_COLOUR);
  Signal newSig = Signal(sig, baseColour.withHue(baseColour.getHue() + ((float)devices.size() / MAX_IDX)));
  signals.push_back(newSig);
  DBG("add sig: " + juce::String(mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, nullptr)));

  // Call signal listeners if device matches
  for (SignalsListener* listener : mSignalsListeners) {
    if (listener->device.dev == device.dev) {
      listener->signalAdded(signals.back());
    }
  }
  // Call devices listener for new signal
  for (DevicesListener* listener : mDevicesListeners) {
    listener->deviceModified(device);
  }

  return newSig;
}

void MapperManager::removeSignal(mpr_sig sig) {
  Device& device = getDevice(sig);
  bool isSource = mpr_obj_get_prop_as_int32(sig, MPR_PROP_DIR, nullptr) == MPR_DIR_OUT;
  std::vector<Signal>& signals = isSource ? device.sourceSignals : device.destSignals;
  
  auto iter = std::find_if(signals.begin(), signals.end(), [sig](Signal other) { return other.sig == sig; });
  if (iter == signals.end()) return; // Skip if not found
  // Call signal listeners if device matches before erasing signal
  for (SignalsListener* listener : mSignalsListeners) {
    if (listener->device.dev == device.dev) {
      listener->signalRemoved(*iter);
    }
  }

  DBG("rem sig: " + juce::String(mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, nullptr)));


  signals.erase(
      std::remove_if(signals.begin(), signals.end(), [sig](Signal other) { return other.sig == sig; }),
      signals.end());

  // Call devices listener for removed signal
  for (DevicesListener* listener : mDevicesListeners) {
    listener->deviceModified(device);
  }
}

void MapperManager::checkAddMap(mpr_map map) {
  mpr_sig sourceSig = *mpr_map_get_sigs(map, MPR_LOC_SRC);
  mpr_sig destSig = *mpr_map_get_sigs(map, MPR_LOC_DST);
  jassert(sourceSig && destSig);
  Signal& sourceSignal = checkAddSignal(sourceSig);
  Signal& destSignal = checkAddSignal(destSig);
  // Skip if already added
  if (std::find_if(maps.begin(), maps.end(), [map](Map other) { return other.map == map; }) != maps.end())
    return;
  maps.push_back(Map(map, &sourceSignal, &destSignal));
}

void MapperManager::removeMap(mpr_map map) {
  std::remove_if(maps.begin(), maps.end(), [map](Map other) { return other.map == map; });
}

MapperManager::Device& MapperManager::getDevice(mpr_sig sig) {
  mpr_dev dev = mpr_sig_get_dev(sig);
  auto devIter =
      std::find_if(devices.begin(), devices.end(), [dev](Device other) { return other.dev == dev; });
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