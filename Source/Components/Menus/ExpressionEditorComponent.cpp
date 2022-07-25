/*
  ==============================================================================

    ExpressionEditorComponent.cpp
    Created: 23 Jul 2022 10:55:08pm
    Author:  brady

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ExpressionEditorComponent.h"

//==============================================================================
ExpressionEditorComponent::ExpressionEditorComponent(MapperManager& manager)
    : mMapperManager(manager) {
  // Buttons
  mBtnApply.setButtonText("Apply");
  mBtnApply.onClick = [this] {
    if (mSelectedMap != nullptr && mExpressionText.getText().isNotEmpty()) {
      mpr_obj_set_prop(mSelectedMap->map, MPR_PROP_EXPR, nullptr, 1, MPR_STR,
                       mExpressionText.getText().getCharPointer(), 1);
      mpr_obj_push(mSelectedMap->map);
      mBtnApply.setEnabled(false);
    }
  };
  mBtnApply.setEnabled(false);
  addAndMakeVisible(mBtnApply);
  mBtnClear.setButtonText("Clear");
  addAndMakeVisible(mBtnClear);

  // Text editor
  mExpressionText.setMultiLine(true, false);
  mExpressionText.setReturnKeyStartsNewLine(true);
  mExpressionText.setFont(
      juce::Font::fromString(juce::Font::getDefaultMonospacedFontName()).withHeight(14));
  mExpressionText.onTextChange = [this] { mBtnApply.setEnabled(true); };
  addAndMakeVisible(mExpressionText);

  mMapperManager.addListener(this);

  setSize(100, 100);
}

ExpressionEditorComponent::~ExpressionEditorComponent() { mMapperManager.removeListener(this); }

void ExpressionEditorComponent::paint(juce::Graphics& g) {
  g.setColour(juce::Colours::white);
  g.drawRoundedRectangle(getLocalBounds().toFloat(), 8, 1.0f);

  g.setColour(juce::Colours::white);
  g.drawText("Expression", getLocalBounds().removeFromTop(20), juce::Justification::centred);
}

void ExpressionEditorComponent::resized() {
  auto r = getLocalBounds().withSizeKeepingCentre(getWidth() - PADDING, getHeight() - PADDING);
  r.removeFromTop(TITLE_HEIGHT);

  // Buttons
  auto btnPanel = r.removeFromLeft(r.getWidth() * 0.15f);
  mBtnApply.setBounds(btnPanel.removeFromTop(r.getHeight() * 0.4f));
  btnPanel.removeFromTop(PADDING);
  mBtnClear.setBounds(btnPanel.removeFromTop(r.getHeight() * 0.4f));

  // Text editor
  mExpressionText.setBounds(r);
}

void ExpressionEditorComponent::updateExpression(MapperManager::Map* map) {
  if (map == nullptr) {
    mExpressionText.setText("");
  } else {
    juce::String expression = mpr_obj_get_prop_as_str(map->map, MPR_PROP_EXPR, nullptr);
    if (expression.isEmpty()) {
      // Must be a new map, show it as a default map
      expression = MapperManager::DEFAULT_MAP_EXPRESSION;
    }
    mExpressionText.setText(expression);
  }
  mSelectedMap = map;
  repaint();
}

void ExpressionEditorComponent::mapAdded(MapperManager::Map* map) { updateExpression(map); }
void ExpressionEditorComponent::mapSelected(MapperManager::Map* map) { updateExpression(map); }
void ExpressionEditorComponent::mapModified(MapperManager::Map* map) { updateExpression(map); }
void ExpressionEditorComponent::mapRemoved(MapperManager::Map* map) {}