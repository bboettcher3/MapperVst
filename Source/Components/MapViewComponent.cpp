/*
  ==============================================================================

    MapViewComponent.cpp
    Created: 16 Oct 2021 9:46:45pm
    Author:  brady

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MapViewComponent.h"

//==============================================================================
MapViewComponent::MapViewComponent(mpr_graph graph) : mGraph(graph) {
  mpr_graph_add_cb(mGraph, deviceCallbackHandler, MPR_DEV, this);
  mpr_graph_add_cb(mGraph, signalCallbackHandler, MPR_SIG, this);
  mpr_graph_add_cb(mGraph, mapCallbackHandler, MPR_MAP, this);

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

MapViewComponent::~MapViewComponent() {
  mpr_graph_remove_cb(mGraph, deviceCallbackHandler, nullptr);
  mpr_graph_remove_cb(mGraph, signalCallbackHandler, nullptr);
  mpr_graph_remove_cb(mGraph, mapCallbackHandler, nullptr);
}

void MapViewComponent::paint(juce::Graphics& g) {
  // Show number of signals
  auto dirLabelPanel = getLocalBounds().removeFromTop(DIR_LABEL_HEIGHT);
  g.setColour(juce::Colours::white);
  juce::String sourcesLabel = juce::String("Sources (") + juce::String(mSourceSigs.size()) + juce::String(")");
  g.drawText(sourcesLabel, dirLabelPanel.removeFromLeft(dirLabelPanel.getWidth() / 2), juce::Justification::left);
  juce::String destsLabel = juce::String("Destinations (") + juce::String(mDestSigs.size()) + juce::String(")");
  g.drawText(destsLabel, dirLabelPanel, juce::Justification::right);

  mpr_dev curDev = nullptr;
  int devYPos = DIR_LABEL_HEIGHT;
  for (int i = 0; i < mSourceSigs.size(); ++i) {
    // Check for new device block
    mpr_dev dev = mpr_sig_get_dev(mSourceSigs[i].sig);
    if (dev != curDev && i > 0) {
      juce::String devName = juce::String(mpr_obj_get_prop_as_str(curDev, MPR_PROP_NAME, nullptr));
      g.setFont(16.0f);
      g.setColour(mSourceSigs[i - 1].colour);
      auto devPanel = juce::Rectangle<int>(0, devYPos, mDevNameWidth, mSourceSigs[i].yPos - devYPos);
      g.fillRect(devPanel);
      g.setColour(juce::Colours::black);
      g.drawText(devName, devPanel, juce::Justification::centred, true);
      g.drawRect(devPanel, 1);
      devYPos = mSourceSigs[i].yPos;
    }
    curDev = dev;
    // Draw signal
    juce::String sigName = juce::String(mpr_obj_get_prop_as_str(mSourceSigs[i].sig, MPR_PROP_NAME, nullptr));
    auto sigPanel = juce::Rectangle<int>(mDevNameWidth, mSourceSigs[i].yPos, mSigWidth, SIG_HEIGHT);
    g.setColour((i % 2) ? mSourceSigs[i].colour.darker(0.1f) : mSourceSigs[i].colour);
    g.fillRect(sigPanel.reduced(1));
    if (mSourceSigs[i].sig == mHoverSig.sig) {
      g.setColour(mSourceSigs[i].colour.brighter(0.2f));
      g.fillRect(sigPanel.reduced(1));
    }
    g.setColour(mSourceSigs[i].sig == mDragSource.sig ? juce::Colours::white : juce::Colours::black);
    g.drawRect(sigPanel, 1);
    g.setColour(juce::Colours::black);
    g.drawText(sigName, sigPanel, juce::Justification::centred);
  }
  // Draw last source device block
  if (mSourceSigs.size() > 0) {
    mpr_dev dev = mpr_sig_get_dev(mSourceSigs.back().sig);
    juce::String devName = juce::String(mpr_obj_get_prop_as_str(dev, MPR_PROP_NAME, nullptr));
    g.setFont(16.0f);
    g.setColour(mSourceSigs.back().colour);
    auto devPanel = juce::Rectangle<int>(0, devYPos, mDevNameWidth, mSourceSigs.back().yPos + SIG_HEIGHT - devYPos);
    g.fillRect(devPanel);
    g.setColour(juce::Colours::black);
    g.drawText(devName, devPanel, juce::Justification::centred, true);
    g.drawRect(devPanel, 1);
  }

  // Dest blocks
  curDev = nullptr;
  devYPos = DIR_LABEL_HEIGHT;
  for (int i = 0; i < mDestSigs.size(); ++i) {
    // Check for new device block
    mpr_dev dev = mpr_sig_get_dev(mDestSigs[i].sig);
    if (dev != curDev && i > 0) {
      juce::String devName = juce::String(mpr_obj_get_prop_as_str(curDev, MPR_PROP_NAME, nullptr));
      g.setFont(16.0f);
      g.setColour(mDestSigs[i - 1].colour);
      auto devPanel = juce::Rectangle<int>(getWidth() - mDevNameWidth, devYPos, mDevNameWidth, mDestSigs[i].yPos - devYPos);
      g.fillRect(devPanel);
      g.setColour(juce::Colours::black);
      g.drawText(devName, devPanel, juce::Justification::centred, true);
      g.drawRect(devPanel, 1);
      devYPos = mDestSigs[i].yPos;
    }
    curDev = dev;
    // Draw signal
    juce::String sigName = juce::String(mpr_obj_get_prop_as_str(mDestSigs[i].sig, MPR_PROP_NAME, nullptr));
    auto sigPanel = juce::Rectangle<int>(getWidth() / 2.0f + MAPPING_GAP, mDestSigs[i].yPos, mSigWidth, SIG_HEIGHT);
    g.setColour((i % 2) ? mDestSigs[i].colour.darker(0.1f) : mDestSigs[i].colour);
    g.fillRect(sigPanel.reduced(1));
    if (mDestSigs[i].sig == mHoverSig.sig) {
      g.setColour(mDestSigs[i].colour.brighter(0.2f));
      g.fillRect(sigPanel.reduced(1));
    }
    g.setColour(mDestSigs[i].sig == mDragDest.sig ? juce::Colours::white : juce::Colours::black);
    g.drawRect(sigPanel, 1);
    g.setColour(juce::Colours::black);
    g.drawText(sigName, sigPanel, juce::Justification::centred);
  }

  // Draw last dest device block
  if (mDestSigs.size() > 0) {
    mpr_dev dev = mpr_sig_get_dev(mDestSigs.back().sig);
    juce::String devName = juce::String(mpr_obj_get_prop_as_str(dev, MPR_PROP_NAME, nullptr));
    g.setFont(16.0f);
    g.setColour(mDestSigs.back().colour);
    auto devPanel = juce::Rectangle<int>(getWidth() - mDevNameWidth, devYPos, mDevNameWidth,
                                         mDestSigs.back().yPos + SIG_HEIGHT - devYPos);
    g.fillRect(devPanel);
    g.setColour(juce::Colours::black);
    g.drawText(devName, devPanel, juce::Justification::centred, true);
    g.drawRect(devPanel, 1);
  }

  // Draw dragging arrow
  if (mDragSource.sig != nullptr) {
    juce::Point<int> dragStart =
        juce::Point<int>(mDevNameWidth + mSigWidth, mDragSource.yPos).translated(0, SIG_HEIGHT / 2.0f);
    g.setColour(juce::Colours::white);
    g.drawArrow(juce::Line<int>(dragStart, mDragPoint).toFloat(), 4, 10, 10);
  }

  // Draw mappings
  for (int i = 0; i < mMaps.size(); ++i) {
    juce::Point<int> connStart =
        juce::Point<int>(mDevNameWidth + mSigWidth, mMaps[i].signals.first.yPos).translated(0, SIG_HEIGHT / 2.0f);
    juce::Point<int> connEnd =
        juce::Point<int>(getWidth() / 2.0f + MAPPING_GAP, mMaps[i].signals.second.yPos).translated(0, SIG_HEIGHT / 2.0f);
    g.setColour(juce::Colours::white);
    g.drawArrow(juce::Line<int>(connStart, connEnd).toFloat(), 4, 10, 10);
  }
}

void MapViewComponent::resized() { mSigWidth = (getWidth() / 2.0f) - MAPPING_GAP - mDevNameWidth; }

void MapViewComponent::checkAddDevice(mpr_dev device) {
  // Skip if already added
  if (std::find(mDevices.begin(), mDevices.end(), device) != mDevices.end()) return;
  mpr_list sigs = mpr_dev_get_sigs(device, MPR_DIR_ANY);
  while (sigs) {
    mpr_sig sig = *sigs;
    checkAddSignal(sig);
    sigs = mpr_list_get_next(sigs);
  }
  repaint();
}

void MapViewComponent::removeDevice(mpr_dev device) {
  std::remove_if(mDevices.begin(), mDevices.end(), [device](mpr_dev other) { return other == device; });
  repaint();
}

void MapViewComponent::checkAddMap(mpr_map map) {
  mpr_sig sourceSig = mpr_map_get_sig(map, 0, MPR_LOC_SRC);
  mpr_sig destSig = mpr_map_get_sig(map, 0, MPR_LOC_DST);
  jassert(sourceSig && destSig);
  Signal sourceSignal = checkAddSignal(sourceSig);
  Signal destSignal = checkAddSignal(destSig);
  // Skip if already added
  if (std::find_if(mMaps.begin(), mMaps.end(), [map](Map other) { return other.map == map; }) != mMaps.end())
    return;
  mMaps.push_back(Map(map, sourceSignal, destSignal));
  repaint();
}

void MapViewComponent::removeMap(mpr_map map) {
  std::remove_if(mMaps.begin(), mMaps.end(), [map](Map other) { return other.map == map; });
  repaint();
}

MapViewComponent::Signal MapViewComponent::checkAddSignal(mpr_sig signal) {
  bool isSource = mpr_obj_get_prop_as_int32(signal, MPR_PROP_DIR, nullptr) == MPR_DIR_OUT;
  std::vector<Signal>& signals = isSource ? mSourceSigs : mDestSigs;
  // Skip if already added
  auto iter = std::find_if(signals.begin(), signals.end(), [signal](Signal other) { return other.sig == signal; });
  if (iter != signals.end())
    return *iter;
  // Add signal to its list
  juce::Colour baseColour = juce::Colour(BASE_COLOUR);
  Signal newSig = Signal(signal, DIR_LABEL_HEIGHT + signals.size() * SIG_HEIGHT,
                         baseColour.withHue(baseColour.getHue() + ((float)mDevices.size() / MAX_IDX)));
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

  if (!foundDevice) mDevices.push_back(inputDevice);

  // Insert signal at the end of its device's list
  signals.insert(sigIter, newSig);
  repaint();
  return newSig;
}

void MapViewComponent::removeSignal(mpr_sig signal) {
  bool isSource = mpr_obj_get_prop_as_int32(signal, MPR_PROP_DIR, nullptr) == MPR_DIR_OUT;
  std::vector<Signal>& signals = isSource ? mSourceSigs : mDestSigs;
  std::remove_if(signals.begin(), signals.end(), [signal](Signal other) { return other.sig == signal; });
}

void MapViewComponent::mouseMove(const juce::MouseEvent& e) {
  mHoverSig = Signal();
  if (e.x < getWidth() / 2.0f) {
    for (Signal& sig : mSourceSigs) {
      if (juce::Rectangle<int>(mDevNameWidth, sig.yPos, mSigWidth, SIG_HEIGHT).contains(e.getPosition())) {
        mHoverSig = sig;
      }
    }
  } else {
    for (Signal& sig : mDestSigs) {
      if (juce::Rectangle<int>((getWidth() / 2.0f) + MAPPING_GAP, sig.yPos, mSigWidth, SIG_HEIGHT)
              .contains(e.getPosition())) {
        mHoverSig = sig;
      }
    }
  }
  repaint();
}

void MapViewComponent::mouseDrag(const juce::MouseEvent& e) {
  // Signal mapping drag
  mDragDest = Signal();
  if (mDragSource.sig != nullptr) {
    mDragPoint = e.getPosition();
    for (Signal& sig : mDestSigs) {
      if (juce::Rectangle<int>((getWidth() / 2.0f) + MAPPING_GAP, sig.yPos, mSigWidth, SIG_HEIGHT)
              .contains(e.getPosition())) {
        mDragDest = sig;
      }
    }
    repaint();
  }
}

void MapViewComponent::mouseExit(const juce::MouseEvent& e) {
  mHoverSig = Signal();
  repaint();
}

void MapViewComponent::mouseDown(const juce::MouseEvent& e) {
  if (mHoverSig.sig != nullptr) {
    mDragSource = mHoverSig;
  }
}

void MapViewComponent::mouseUp(const juce::MouseEvent& e) {
  if (mDragSource.sig != nullptr && mDragDest.sig != nullptr) {
    // Add new mapping
    mpr_map newMap = mpr_map_new(1, &mDragSource.sig, 1, &mDragDest.sig);
    mpr_obj_push(newMap);
    mMaps.push_back(Map(newMap, mDragSource, mDragDest));
    mDragSource = Signal();
    mDragDest = Signal();
    mDragPoint = juce::Point<int>();
  }
  repaint();
}

void MapViewComponent::deviceCallbackHandler(mpr_graph g, mpr_dev dev, mpr_graph_evt e, const void* context) {
  if (context == nullptr) return;
  MapViewComponent* component = (MapViewComponent*)context;
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

void MapViewComponent::signalCallbackHandler(mpr_graph g, mpr_sig sig, mpr_graph_evt e, const void* context) {
  if (context == nullptr) return;
  MapViewComponent* component = (MapViewComponent*)context;
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

void MapViewComponent::mapCallbackHandler(mpr_graph g, mpr_map map, mpr_graph_evt e, const void* context) {
  if (context == nullptr) return;
  MapViewComponent* component = (MapViewComponent*)context;
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