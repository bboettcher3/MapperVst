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

  class Signal {
   public:
    Signal() : sig(nullptr) {}
    Signal(mpr_sig sig) : sig(sig) {}
    bool operator==(const Signal& s) { return (sig == s.sig); }
    juce::String getDescription(bool isDetailed);
    mpr_sig sig;
  };

  class Device {
   public:
    Device() : dev(nullptr) {}
    Device(mpr_dev dev, juce::Colour colour) : dev(dev), colour(colour) {}
    bool operator==(const Device& d) { return (dev == d.dev); }
    mpr_dev dev;
    juce::Colour colour;
    juce::OwnedArray<Signal> sourceSignals;
    juce::OwnedArray<Signal> destSignals;
  };

  class Map {
   public:
    Map() : map(nullptr) {}
    Map(mpr_map map, std::vector<Signal*> sourceSignals, std::vector<Signal*> destSignals)
        : map(map), sourceSignals(sourceSignals), destSignals(destSignals) {}
    bool operator==(const Map& m) { return (map == m.map); }
    mpr_map map;
    std::vector<Signal*> sourceSignals;
    std::vector<Signal*> destSignals;
  };

  // Listens to device changes in the graph
  class DevicesListener {
   public:
    virtual void deviceAdded(Device* device) = 0;
    /* Signal added or removed or properties changed in device */
    virtual void deviceModified(Device* device) = 0;
    virtual void deviceRemoved(Device* device) = 0;
  };

  // Listens to signal changes for a single device
  class SignalsListener {
   public:
    SignalsListener(Device* device) : device(device) {}
    virtual void signalAdded(Signal* signal) = 0;
    virtual void signalRemoved(Signal* signal) = 0;
    Device* device;
  };

  // Listens to map changes in the graph
  class MapsListener {
   public:
    virtual void mapAdded(Map* map) = 0;
    virtual void mapSelected(Map* map) = 0;
    virtual void mapModified(Map* map) = 0;
    virtual void mapRemoved(Map* map) = 0;
    Map* map;
  };

  void addListener(DevicesListener* newListener);
  void addListener(SignalsListener* newListener);
  void addListener(MapsListener* newListener);
  void removeListener(DevicesListener* listener);
  void removeListener(SignalsListener* listener);
  void removeListener(MapsListener* listener);

  /* Graph changes */
  void refreshGraph();
  // Conditionally adds device and its signals if not already added
  void checkAddDevice(mpr_dev dev);
  void removeDevice(mpr_dev dev);
  // Conditionally adds signal to respective list if not already added
  Signal* checkAddSignal(mpr_sig sig);
  void removeSignal(mpr_sig sig);
  // Conditionally adds mapping if not already added
  Map* checkAddMap(mpr_map map);
  void removeMap(mpr_map map);

  /* Global view selections */
  void setSelectedMap(Map* map);

  static constexpr auto DEFAULT_MAP_EXPRESSION = "y=x";

  mpr_graph graph;  // libmapper graph that keeps track of network (created by processor)
  juce::OwnedArray<Device> devices;
  juce::OwnedArray<Map> maps;

 private:
  static constexpr auto MAX_IDX = 20;

  // Bookkeeping
  juce::CriticalSection mListenerLock;
  juce::Array<DevicesListener*> mDevicesListeners;
  juce::Array<SignalsListener*> mSignalsListeners;
  juce::Array<MapsListener*> mMapsListeners;
  int mCurDeviceColourIdx = 1;
  Map* mSelectedMap = nullptr;

  Device* getDevice(mpr_sig sig);
  static void deviceCallbackHandler(mpr_graph g, mpr_dev dev, mpr_graph_evt e, const void* context);
  static void signalCallbackHandler(mpr_graph g, mpr_sig sig, mpr_graph_evt e, const void* context);
  static void mapCallbackHandler(mpr_graph g, mpr_map map, mpr_graph_evt e, const void* context);

  static juce::String getTypeString(int type);
};
