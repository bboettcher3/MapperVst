/*
  ==============================================================================

    ExpressionEditorComponent.h
    Created: 23 Jul 2022 10:55:08pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
 */
class ExpressionEditorComponent : public juce::Component {
 public:
  ExpressionEditorComponent();
  ~ExpressionEditorComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

 private:
  static constexpr int TITLE_HEIGHT = 14;
  static constexpr int PADDING = 10;

  juce::TextButton mBtnApply;
  juce::TextButton mBtnClear;
  juce::TextEditor mExpressionText;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExpressionEditorComponent)
};
