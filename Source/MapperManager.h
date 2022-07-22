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

  typedef struct Device {
    Device() : dev(nullptr) {}
    Device(mpr_dev dev, juce::Colour colour) : dev(dev), colour(colour) {}
    mpr_dev dev;
    juce::Colour colour;
    std::vector<Signal> sourceSignals;
    std::vector<Signal> destSignals;
  } Device;

  typedef struct Map {
    Map() : map(nullptr) {}
    Map(mpr_map map, Signal* sourceSignal, Signal* destSignal)
        : map(map), signals(sourceSignal, destSignal) {}
    mpr_map map;
    std::pair<Signal*, Signal*> signals;
  } Map;

  // Listens to device changes in the graph
  class DevicesListener {
   public:
    virtual void deviceAdded(Device& device) = 0;
    /* Signal added or removed or properties changed in device */
    virtual void deviceModified(Device& device) = 0;
    virtual void deviceRemoved(Device& device) = 0;
  };

  // Listens to signal changes for a single device
  class SignalsListener {
   public:
    SignalsListener(Device& device) : device(device) {}
    virtual void signalAdded(Signal& signal) = 0;
    virtual void signalRemoved(Signal& signal) = 0;
    
    Device& device;
  };

  void addListener(DevicesListener* newListener);
  void addListener(SignalsListener* newListener);
  void removeListener(DevicesListener* listener);
  void removeListener(SignalsListener* listener);

  /* Graph changes */
  void refreshGraph();
  // Conditionally adds device and its signals if not already added
  void checkAddDevice(mpr_dev dev);
  void removeDevice(mpr_dev dev);
  // Conditionally adds signal to respective list if not already added
  Signal& checkAddSignal(mpr_sig sig);
  void removeSignal(mpr_sig sig);
  // Conditionally adds mapping if not already added
  Map& checkAddMap(mpr_map map);
  void removeMap(mpr_map map);

  mpr_graph graph;  // libmapper graph that keeps track of network (created by processor)
  std::vector<Device> devices;
  std::vector<Map> maps;

 private:
  static constexpr auto BASE_COLOUR = 0xFF52B85F;
  static constexpr auto MAX_IDX = 20;

  // Bookkeeping
  juce::Array<DevicesListener*> mDevicesListeners;
  juce::Array<SignalsListener*> mSignalsListeners;

  Device& getDevice(mpr_sig sig);
  static void deviceCallbackHandler(mpr_graph g, mpr_dev dev, mpr_graph_evt e, const void* context);
  static void signalCallbackHandler(mpr_graph g, mpr_sig sig, mpr_graph_evt e, const void* context);
  static void mapCallbackHandler(mpr_graph g, mpr_map map, mpr_graph_evt e, const void* context);
};
