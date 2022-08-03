/*
  ==============================================================================

    FileOpenSaveComponent.h
    Created: 20 Jul 2022 4:00:09pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../MapperManager.h"

//==============================================================================
/*
 */
class FileOpenSaveComponent : public juce::Component {
 public:
  FileOpenSaveComponent(MapperManager& manager);
  ~FileOpenSaveComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

 private:
  static constexpr int FONT_HEIGHT = 14;
  static constexpr int PADDING = 10;

  // Bookkeeping
  MapperManager& mMapperManager;
  std::unique_ptr<juce::FileChooser> mFileChooser;

  // Components
  juce::TextButton mBtnOpen;
  juce::TextButton mBtnSave;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileOpenSaveComponent)
};
