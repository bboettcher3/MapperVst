/*
  ==============================================================================

    MapViewComponent.h
    Created: 16 Oct 2021 9:46:45pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "mapper/mapper.h"

//==============================================================================
/*
 */
class MapViewComponent : public juce::Component {
 public:
  MapViewComponent(mpr_graph graph);
  ~MapViewComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  void mouseMove(const juce::MouseEvent& e) override;
  void mouseDrag(const juce::MouseEvent& e) override;
  void mouseDown(const juce::MouseEvent& e) override;
  void mouseUp(const juce::MouseEvent& e) override;
  void mouseExit(const juce::MouseEvent& e) override;

  int getNumSourceSigs() { return mSourceSigs.size(); }
  int getNumDestSigs() { return mDestSigs.size(); }

 private:
  static constexpr auto DIR_LABEL_HEIGHT = 20;
  static constexpr auto BASE_COLOUR = 0xFF52B85F;
  static constexpr auto MAX_IDX = 20;
  static constexpr auto DEFAULT_DEV_NAME_WIDTH = 100;
  static constexpr auto SIG_HEIGHT = 40;
  static constexpr auto MAPPING_GAP = 100;

  typedef struct Signal {
    Signal() : sig(nullptr) {}
    Signal(mpr_sig sig, int yPos, juce::Colour colour) : sig(sig), yPos(yPos), colour(colour) {}
    mpr_sig sig;
    int yPos;
    juce::Colour colour;
  } Signal;

  typedef struct Map {
    Map() : map(nullptr) {}
    Map(mpr_map map, Signal sourceSig, Signal destSig) : map(map), signals(sourceSig, destSig) {}
    mpr_map map;
    std::pair<Signal, Signal> signals;
  } Map;

  /* Graph changes */
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
    mSourceSigs.clear();
    mDestSigs.clear();
    mDevices.clear();
  }

  static void deviceCallbackHandler(mpr_graph g, mpr_dev dev, mpr_graph_evt e, const void* context);
  static void signalCallbackHandler(mpr_graph g, mpr_sig sig, mpr_graph_evt e, const void* context);
  static void mapCallbackHandler(mpr_graph g, mpr_map map, mpr_graph_evt e, const void* context);

  // Bookkeeping
  mpr_graph mGraph;  // libmapper graph that keeps track of network (created by processor)
  std::vector<mpr_dev> mDevices;
  std::vector<Signal> mSourceSigs;
  std::vector<Signal> mDestSigs;
  std::vector<Map> mMaps;
  Signal mDragSource, mDragDest;  // Only non-null sig if currently dragging
  Signal mHoverSig;               // Index of sig being hovered over
  juce::Point<int> mDragPoint;
  int mDevNameWidth = DEFAULT_DEV_NAME_WIDTH;
  int mSigWidth;  // Adjusted in resized() relative to dev name width

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapViewComponent)
};
