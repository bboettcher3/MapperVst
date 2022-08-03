/*
  ==============================================================================

    FileOpenSaveComponent.cpp
    Created: 20 Jul 2022 4:00:09pm
    Author:  brady

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FileOpenSaveComponent.h"
#include "../../Presets.h"

//==============================================================================
FileOpenSaveComponent::FileOpenSaveComponent(MapperManager& manager) : mMapperManager(manager) {
  mBtnOpen.setButtonText("Open");
  mBtnOpen.onClick = [this] {
    mFileChooser = std::make_unique<juce::FileChooser>(
        "Please select the mapping preset you want to load...",
        juce::File::getSpecialLocation(juce::File::currentApplicationFile), "*.json");

    auto folderChooserFlags =
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    mFileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& chooser) {
      juce::File presetFile(chooser.getResult());

      Preset::open(presetFile, mMapperManager.maps.data(), mMapperManager.maps.size());
    });
  };
  addAndMakeVisible(mBtnOpen);
  mBtnSave.setButtonText("Save");
  mBtnSave.onClick = [this] {
    mFileChooser = std::make_unique<juce::FileChooser>(
        "Please select or create a file to save the mapping preset into...",
        juce::File::getSpecialLocation(juce::File::currentApplicationFile)
            .getParentDirectory()
            .getNonexistentChildFile("mappingPreset", "", false),
        "*.json");

    auto folderChooserFlags = juce::FileBrowserComponent::saveMode |
                              juce::FileBrowserComponent::canSelectFiles |
                              juce::FileBrowserComponent::warnAboutOverwriting |
                              juce::FileBrowserComponent::doNotClearFileNameOnRootChange;

    mFileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& chooser) {
      juce::File presetFile(chooser.getResult());

      Preset::save(presetFile, mMapperManager.maps.data(), mMapperManager.maps.size());
    });
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
