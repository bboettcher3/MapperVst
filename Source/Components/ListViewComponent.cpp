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
ListViewComponent::ListViewComponent(MapperManager manager) : mMapperManager(manager) {
}

void ListViewComponent::paint(juce::Graphics& g) {
  // Show number of signals
  auto dirLabelPanel = getLocalBounds().removeFromTop(DIR_LABEL_HEIGHT);
  g.setColour(juce::Colours::white);
  juce::String sourcesLabel = juce::String("Sources (") + juce::String(mMapperManager.sourceSigs.size()) + juce::String(")");
  g.drawText(sourcesLabel, dirLabelPanel.removeFromLeft(dirLabelPanel.getWidth() / 2), juce::Justification::left);
  juce::String destsLabel = juce::String("Destinations (") + juce::String(mMapperManager.destSigs.size()) + juce::String(")");
  g.drawText(destsLabel, dirLabelPanel, juce::Justification::right);

  mpr_dev curDev = nullptr;
  int devYPos = DIR_LABEL_HEIGHT;
  for (int i = 0; i < mMapperManager.sourceSigs.size(); ++i) {
    // Check for new device block
    mpr_dev dev = mpr_sig_get_dev(mMapperManager.sourceSigs[i].sig);
    if (dev != curDev && i > 0) {
      juce::String devName = juce::String(mpr_obj_get_prop_as_str(curDev, MPR_PROP_NAME, nullptr));
      g.setFont(16.0f);
      g.setColour(mMapperManager.sourceSigs[i - 1].colour);
      auto devPanel = juce::Rectangle<int>(0, devYPos, mDevNameWidth, mMapperManager.sourceSigs[i].yPos - devYPos);
      g.fillRect(devPanel);
      g.setColour(juce::Colours::black);
      g.drawText(devName, devPanel, juce::Justification::centred, true);
      g.drawRect(devPanel, 1);
      devYPos = mMapperManager.sourceSigs[i].yPos;
    }
    curDev = dev;
    // Draw signal
    juce::String sigName = juce::String(mpr_obj_get_prop_as_str(mMapperManager.sourceSigs[i].sig, MPR_PROP_NAME, nullptr));
    auto sigPanel = juce::Rectangle<int>(mDevNameWidth, mMapperManager.sourceSigs[i].yPos, mSigWidth, SIG_HEIGHT);
    g.setColour((i % 2) ? mMapperManager.sourceSigs[i].colour.darker(0.1f) : mMapperManager.sourceSigs[i].colour);
    g.fillRect(sigPanel.reduced(1));
    if (mMapperManager.sourceSigs[i].sig == mHoverSig.sig) {
      g.setColour(mMapperManager.sourceSigs[i].colour.brighter(0.2f));
      g.fillRect(sigPanel.reduced(1));
    }
    g.setColour(mMapperManager.sourceSigs[i].sig == mDragSource.sig ? juce::Colours::white : juce::Colours::black);
    g.drawRect(sigPanel, 1);
    g.setColour(juce::Colours::black);
    g.drawText(sigName, sigPanel, juce::Justification::centred);
  }
  // Draw last source device block
  if (mMapperManager.sourceSigs.size() > 0) {
    mpr_dev dev = mpr_sig_get_dev(mMapperManager.sourceSigs.back().sig);
    juce::String devName = juce::String(mpr_obj_get_prop_as_str(dev, MPR_PROP_NAME, nullptr));
    g.setFont(16.0f);
    g.setColour(mMapperManager.sourceSigs.back().colour);
    auto devPanel =
        juce::Rectangle<int>(0, devYPos, mDevNameWidth, mMapperManager.sourceSigs.back().yPos + SIG_HEIGHT - devYPos);
    g.fillRect(devPanel);
    g.setColour(juce::Colours::black);
    g.drawText(devName, devPanel, juce::Justification::centred, true);
    g.drawRect(devPanel, 1);
  }

  // Dest blocks
  curDev = nullptr;
  devYPos = DIR_LABEL_HEIGHT;
  for (int i = 0; i < mMapperManager.destSigs.size(); ++i) {
    // Check for new device block
    mpr_dev dev = mpr_sig_get_dev(mMapperManager.destSigs[i].sig);
    if (dev != curDev && i > 0) {
      juce::String devName = juce::String(mpr_obj_get_prop_as_str(curDev, MPR_PROP_NAME, nullptr));
      g.setFont(16.0f);
      g.setColour(mMapperManager.destSigs[i - 1].colour);
      auto devPanel = juce::Rectangle<int>(getWidth() - mDevNameWidth, devYPos, mDevNameWidth, mMapperManager.destSigs[i].yPos - devYPos);
      g.fillRect(devPanel);
      g.setColour(juce::Colours::black);
      g.drawText(devName, devPanel, juce::Justification::centred, true);
      g.drawRect(devPanel, 1);
      devYPos = mMapperManager.destSigs[i].yPos;
    }
    curDev = dev;
    // Draw signal
    juce::String sigName = juce::String(mpr_obj_get_prop_as_str(mMapperManager.destSigs[i].sig, MPR_PROP_NAME, nullptr));
    auto sigPanel = juce::Rectangle<int>(getWidth() / 2.0f + MAPPING_GAP, mMapperManager.destSigs[i].yPos, mSigWidth, SIG_HEIGHT);
    g.setColour((i % 2) ? mMapperManager.destSigs[i].colour.darker(0.1f) : mMapperManager.destSigs[i].colour);
    g.fillRect(sigPanel.reduced(1));
    if (mMapperManager.destSigs[i].sig == mHoverSig.sig) {
      g.setColour(mMapperManager.destSigs[i].colour.brighter(0.2f));
      g.fillRect(sigPanel.reduced(1));
    }
    g.setColour(mMapperManager.destSigs[i].sig == mDragDest.sig ? juce::Colours::white : juce::Colours::black);
    g.drawRect(sigPanel, 1);
    g.setColour(juce::Colours::black);
    g.drawText(sigName, sigPanel, juce::Justification::centred);
  }

  // Draw last dest device block
  if (mMapperManager.destSigs.size() > 0) {
    mpr_dev dev = mpr_sig_get_dev(mMapperManager.destSigs.back().sig);
    juce::String devName = juce::String(mpr_obj_get_prop_as_str(dev, MPR_PROP_NAME, nullptr));
    g.setFont(16.0f);
    g.setColour(mMapperManager.destSigs.back().colour);
    auto devPanel = juce::Rectangle<int>(getWidth() - mDevNameWidth, devYPos, mDevNameWidth,
                                         mMapperManager.destSigs.back().yPos + SIG_HEIGHT - devYPos);
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
  for (int i = 0; i < mMapperManager.maps.size(); ++i) {
    juce::Point<int> connStart = juce::Point<int>(mDevNameWidth + mSigWidth, mMapperManager.maps[i].signals.first.yPos)
                                     .translated(0, SIG_HEIGHT / 2.0f);
    juce::Point<int> connEnd = juce::Point<int>(getWidth() / 2.0f + MAPPING_GAP, mMapperManager.maps[i].signals.second.yPos)
                                   .translated(0, SIG_HEIGHT / 2.0f);
    g.setColour(juce::Colours::white);
    g.drawArrow(juce::Line<int>(connStart, connEnd).toFloat(), 4, 10, 10);
  }
}

void ListViewComponent::resized() { mSigWidth = (getWidth() / 2.0f) - MAPPING_GAP - mDevNameWidth; }

void ListViewComponent::mouseMove(const juce::MouseEvent& e) {
  mHoverSig = MapperManager::Signal();
  if (e.x < getWidth() / 2.0f) {
    for (MapperManager::Signal& sig : mMapperManager.sourceSigs) {
      if (juce::Rectangle<int>(mDevNameWidth, sig.yPos, mSigWidth, SIG_HEIGHT).contains(e.getPosition())) {
        mHoverSig = sig;
      }
    }
  } else {
    for (MapperManager::Signal& sig : mMapperManager.destSigs) {
      if (juce::Rectangle<int>((getWidth() / 2.0f) + MAPPING_GAP, sig.yPos, mSigWidth, SIG_HEIGHT)
              .contains(e.getPosition())) {
        mHoverSig = sig;
      }
    }
  }
  repaint();
}

void ListViewComponent::mouseDrag(const juce::MouseEvent& e) {
  // Signal mapping drag
  mDragDest = MapperManager::Signal();
  if (mDragSource.sig != nullptr) {
    mDragPoint = e.getPosition();
    for (MapperManager::Signal& sig : mDestSigs) {
      if (juce::Rectangle<int>((getWidth() / 2.0f) + MAPPING_GAP, sig.yPos, mSigWidth, SIG_HEIGHT)
              .contains(e.getPosition())) {
        mDragDest = sig;
      }
    }
    repaint();
  }
}

void ListViewComponent::mouseExit(const juce::MouseEvent& e) {
  mHoverSig = MapperManager::Signal();
  repaint();
}

void ListViewComponent::mouseDown(const juce::MouseEvent& e) {
  if (mHoverSig.sig != nullptr) {
    mDragSource = mHoverSig;
  }
}

void ListViewComponent::mouseUp(const juce::MouseEvent& e) {
  if (mDragSource.sig != nullptr && mDragDest.sig != nullptr) {
    // Add new mapping
    mpr_map newMap = mpr_map_new(1, &mDragSource.sig, 1, &mDragDest.sig);
    mpr_obj_push(newMap);
    mMapperManager.checkAddMap(newMap);
    mDragSource = MapperManager::Signal();
    mDragDest = MapperManager::Signal();
    mDragPoint = juce::Point<int>();
  }
  repaint();
}