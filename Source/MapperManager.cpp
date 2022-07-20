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

void MapperManager::refreshGraph() {
  // Sync graph with network
  mpr_graph_poll(graph, 500);
}

void MapperManager::checkAddDevice(mpr_dev device) {
  // Skip if already added
  if (std::find(devices.begin(), devices.end(), device) != devices.end()) return;
  mpr_list sigs = mpr_dev_get_sigs(device, MPR_DIR_ANY);
  while (sigs) {
    mpr_sig sig = *sigs;
    checkAddSignal(sig);
    sigs = mpr_list_get_next(sigs);
  }
}

void MapperManager::removeDevice(mpr_dev device) {
  std::remove_if(devices.begin(), devices.end(), [device](mpr_dev other) { return other == device; });
}

void MapperManager::checkAddMap(mpr_map map) {
  mpr_sig sourceSig = *mpr_map_get_sigs(map, MPR_LOC_SRC);
  mpr_sig destSig = *mpr_map_get_sigs(map, MPR_LOC_DST);
  jassert(sourceSig && destSig);
  Signal sourceSignal = checkAddSignal(sourceSig);
  Signal destSignal = checkAddSignal(destSig);
  // Skip if already added
  if (std::find_if(maps.begin(), maps.end(), [map](Map other) { return other.map == map; }) != maps.end()) return;
  maps.push_back(Map(map, sourceSignal, destSignal));
}

void MapperManager::removeMap(mpr_map map) {
  std::remove_if(maps.begin(), maps.end(), [map](Map other) { return other.map == map; });
}

MapperManager::Signal MapperManager::checkAddSignal(mpr_sig signal) {
  bool isSource = mpr_obj_get_prop_as_int32(signal, MPR_PROP_DIR, nullptr) == MPR_DIR_OUT;
  std::vector<Signal>& signals = isSource ? sourceSigs : destSigs;
  // Skip if already added
  auto iter = std::find_if(signals.begin(), signals.end(), [signal](Signal other) { return other.sig == signal; });
  if (iter != signals.end()) return *iter;
  // Add signal to its list
  juce::Colour baseColour = juce::Colour(BASE_COLOUR);
  Signal newSig = Signal(signal, baseColour.withHue(baseColour.getHue() + ((float)devices.size() / MAX_IDX)));
  // Find where to insert it in list (next to signals with same device)
  mpr_dev inputDevice = mpr_sig_get_dev(signal);
  bool foundDevice = false;
  auto sigIter = signals.begin();
  for (; sigIter != signals.end(); ++sigIter) {
    // Break only after finding device
    if (mpr_sig_get_dev((*sigIter).sig) == inputDevice) {
      foundDevice = true;
    } else if (foundDevice) {
      break;
    }
  }

  if (!foundDevice) devices.push_back(inputDevice);

  // Insert signal at the end of its device's list
  signals.insert(sigIter, newSig);
  return newSig;
}

void MapperManager::removeSignal(mpr_sig signal) {
  bool isSource = mpr_obj_get_prop_as_int32(signal, MPR_PROP_DIR, nullptr) == MPR_DIR_OUT;
  std::vector<Signal>& signals = isSource ? sourceSigs : destSigs;
  std::remove_if(signals.begin(), signals.end(), [signal](Signal other) { return other.sig == signal; });
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