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
MapViewComponent::MapViewComponent() {
  // In your constructor, you should add any child components, and
  // initialise any special settings that your component needs.
}

MapViewComponent::~MapViewComponent() {}

void MapViewComponent::paint(juce::Graphics& g) {
  mpr_dev curDev = nullptr;
  int devYPos = 0;
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
  devYPos = 0;
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
    g.setColour(mDestSigs[i].sig == mDragSource.sig ? juce::Colours::white : juce::Colours::black);
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
    auto devPanel =
        juce::Rectangle<int>(getWidth() - mDevNameWidth, devYPos, mDevNameWidth, mDestSigs.back().yPos + SIG_HEIGHT - devYPos);
    g.fillRect(devPanel);
    g.setColour(juce::Colours::black);
    g.drawText(devName, devPanel, juce::Justification::centred, true);
    g.drawRect(devPanel, 1);
  }
  
  // Draw dragging arrow
  if (mDragSource.sig != nullptr) {
    juce::Point<int> dragStart = juce::Point<int>(mDevNameWidth + mSigWidth, mDragSource.yPos).translated(0, SIG_HEIGHT / 2.0f);
    g.setColour(juce::Colours::white);
    g.drawArrow(juce::Line<int>(dragStart, mDragPoint).toFloat(), 4, 10, 10);
  }
}

void MapViewComponent::resized() { mSigWidth = (getWidth() / 2.0f) - MAPPING_GAP - mDevNameWidth; }

void MapViewComponent::addDevice(mpr_dev device) {
  mpr_list sigs = mpr_dev_get_sigs(device, MPR_DIR_ANY);
  juce::Colour baseColour = juce::Colour(BASE_COLOUR);
  while (sigs) {
    mpr_sig sig = *sigs;
    bool isSourceSig = mpr_obj_get_prop_as_int32(sig, MPR_PROP_DIR, nullptr) == MPR_DIR_OUT;
    if (isSourceSig) {
      Signal newSig =
          Signal(sig, mSourceSigs.size() * SIG_HEIGHT, baseColour.withHue(baseColour.getHue() + ((float)mNumDevices / MAX_IDX)));
      mSourceSigs.push_back(newSig);
    } else {
      Signal newSig =
          Signal(sig, mDestSigs.size() * SIG_HEIGHT, baseColour.withHue(baseColour.getHue() + ((float)mNumDevices / MAX_IDX)));
      mDestSigs.push_back(newSig);
    }    
    sigs = mpr_list_get_next(sigs);
  }
  mNumDevices++;
  repaint();
}

void MapViewComponent::mouseMove(const juce::MouseEvent& e) {
  mHoverSig = Signal();
  if (e.x < getWidth() / 2.0f) {
    for (Signal& sig : mSourceSigs) {
      if (juce::Rectangle<int>(mDevNameWidth, sig.yPos, mSigWidth, SIG_HEIGHT).contains(e.getPosition())) {
        mHoverSig = sig;
        repaint();
      }
    }
  } else {
    for (Signal& sig : mDestSigs) {
      if (juce::Rectangle<int>((getWidth() / 2.0f) + MAPPING_GAP, sig.yPos, mSigWidth, SIG_HEIGHT).contains(e.getPosition())) {
        mHoverSig = sig;
        repaint();
      }
    }
  }
}

void MapViewComponent::mouseDrag(const juce::MouseEvent& e) {
  // Signal mapping drag
  if (mDragSource.sig != nullptr) {
    mDragPoint = e.getPosition();
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
  if (mDragSource.sig != nullptr) {
    mDragSource = Signal();
    mDragPoint = juce::Point<int>();
    repaint();
  }
}