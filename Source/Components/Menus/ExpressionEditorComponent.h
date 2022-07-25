/*
  ==============================================================================

    ExpressionEditorComponent.h
    Created: 23 Jul 2022 10:55:08pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../MapperManager.h"

//==============================================================================
/*
 */
class ExpressionEditorComponent : public juce::Component, MapperManager::MapsListener {
 public:
  ExpressionEditorComponent(MapperManager& manager);
  ~ExpressionEditorComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  void mapAdded(MapperManager::Map* map) override;
  void mapSelected(MapperManager::Map* map) override;
  void mapModified(MapperManager::Map* map) override;
  void mapRemoved(MapperManager::Map* map) override;

 private:
  static constexpr int TITLE_HEIGHT = 14;
  static constexpr int PADDING = 10;

  // Bookkeeping
  MapperManager& mMapperManager;
  MapperManager::Map* mSelectedMap = nullptr;

  // Components
  juce::TextButton mBtnApply;
  juce::TextButton mBtnClear;
  juce::TextEditor mExpressionText;

  void updateExpression(MapperManager::Map* map);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExpressionEditorComponent)
};
