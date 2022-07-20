/*
  ==============================================================================

    FileOpenSaveComponent.cpp
    Created: 20 Jul 2022 4:00:09pm
    Author:  brady

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FileOpenSaveComponent.h"

//==============================================================================
FileOpenSaveComponent::FileOpenSaveComponent() {
  mBtnOpen.setButtonText("Open");
  mBtnOpen.onClick = [this] {
    if (onOpenClicked != nullptr) {
      onOpenClicked();
    }
  };
  addAndMakeVisible(mBtnOpen);
  mBtnSave.setButtonText("Save");
  mBtnSave.onClick = [this] {
    if (onSaveClicked != nullptr) {
      onSaveClicked();
    }
  };
  addAndMakeVisible(mBtnSave);

  setSize(100, 100);
}

FileOpenSaveComponent::~FileOpenSaveComponent() {}

void FileOpenSaveComponent::paint(juce::Graphics& g) {
  g.setColour(juce::Colours::white);
  g.drawRoundedRectangle(getLocalBounds().toFloat(), 8, 1.0f);
  
  g.setColour(juce::Colours::white);
  g.drawText("File", getLocalBounds().removeFromTop(20), juce::Justification::centred);
}

void FileOpenSaveComponent::resized() {
  auto r = getLocalBounds().withSizeKeepingCentre(getWidth() - PADDING, getHeight() - PADDING);
  r.removeFromTop(FONT_HEIGHT);

  auto btnOpenBounds = r.removeFromTop(r.getHeight() / 2.0f)
                           .withSizeKeepingCentre(r.getWidth() - PADDING, r.getHeight() / 2.0f - PADDING);
  mBtnOpen.setBounds(btnOpenBounds);
  mBtnSave.setBounds(r.withSizeKeepingCentre(btnOpenBounds.getWidth(), btnOpenBounds.getHeight()));
}
