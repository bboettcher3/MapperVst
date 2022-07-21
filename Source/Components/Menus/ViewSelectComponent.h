/*
  ==============================================================================

    ViewSelectComponent.h
    Created: 20 Jul 2022 5:06:27pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
 */
class ViewSelectComponent : public juce::Component {
 public:
  ViewSelectComponent();
  ~ViewSelectComponent() override;

  class ViewType {
    ViewType(juce::String typeName, juce::String typeIconFilename) : typeName(typeName), typeIconFilename(typeIconFilename) {}
    juce::String typeName;
    juce::String typeIconFilename;
  };

  void paint(juce::Graphics&) override;
  void resized() override;

  // std::array<ViewType, 1> viewTypes = {ViewType("List View")};

 private:
  static constexpr int PADDING = 10;
  static constexpr int ICON_SIZE = 20;

  // ViewType mViewType = ViewType::DEVICE;
  juce::ImageButton mBtnListView;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ViewSelectComponent)
};
