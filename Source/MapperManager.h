/*
  ==============================================================================

    MapperManager.h
    Created: 20 Jul 2022 7:12:43pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "mapper/mapper.h"

class MapperManager {
 public:
  MapperManager();
  ~MapperManager();

  typedef struct Signal {
    Signal() : sig(nullptr) {}
    Signal(mpr_sig sig, juce::Colour colour) : sig(sig), colour(colour) {}
    mpr_sig sig;
    juce::Colour colour;
  } Signal;

  typedef struct Map {
    Map() : map(nullptr) {}
    Map(mpr_map map, Signal sourceSig, Signal destSig) : map(map), signals(sourceSig, destSig) {}
    mpr_map map;
    std::pair<Signal, Signal> signals;
  } Map;

  int getNumSourceSigs() { return sourceSigs.size(); }
  int getNumDestSigs() { return destSigs.size(); }
  
  /* Graph changes */
  void refreshGraph();
  // Conditionally adds mapping if not already added
  void checkAddMap(mpr_map map);
  void removeMap(mpr_map map);
  // Conditionally adds device and its signals if not already added
  void checkAddDevice(mpr_dev device);
  void removeDevice(mpr_dev device);
  // Conditionally adds signal to respective list if not already added
  Signal checkAddSignal(mpr_sig signal);
  void removeSignal(mpr_sig signal);
  void clearDevices() {
    sourceSigs.clear();
    destSigs.clear();
    devices.clear();
  }

  mpr_graph graph;  // libmapper graph that keeps track of network (created by processor)
  std::vector<mpr_dev> devices;
  std::vector<Signal> sourceSigs;
  std::vector<Signal> destSigs;
  std::vector<Map> maps;

 private:
  static void deviceCallbackHandler(mpr_graph g, mpr_dev dev, mpr_graph_evt e, const void* context);
  static void signalCallbackHandler(mpr_graph g, mpr_sig sig, mpr_graph_evt e, const void* context);
  static void mapCallbackHandler(mpr_graph g, mpr_map map, mpr_graph_evt e, const void* context);

  static constexpr auto BASE_COLOUR = 0xFF52B85F;
  static constexpr auto MAX_IDX = 20;
};
