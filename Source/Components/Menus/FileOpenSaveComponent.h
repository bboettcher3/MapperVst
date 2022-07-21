/*
  ==============================================================================

    FileOpenSaveComponent.h
    Created: 20 Jul 2022 4:00:09pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
 */
class FileOpenSaveComponent : public juce::Component {
 public:
  FileOpenSaveComponent();
  ~FileOpenSaveComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  std::function<void()> onOpenClicked = nullptr;
  std::function<void()> onSaveClicked = nullptr;

 private:
  static constexpr int FONT_HEIGHT = 14;
  static constexpr int PADDING = 10;

  juce::TextButton mBtnOpen;
  juce::TextButton mBtnSave;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileOpenSaveComponent)
};
