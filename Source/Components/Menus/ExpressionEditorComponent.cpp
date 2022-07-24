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
ExpressionEditorComponent::ExpressionEditorComponent() {

    // Buttons
    mBtnApply.setButtonText("Apply");
    addAndMakeVisible(mBtnApply);
    mBtnClear.setButtonText("Clear"); 
    addAndMakeVisible(mBtnClear);

    // Text editor
    mExpressionText.setMultiLine(true, false);
    mExpressionText.setReturnKeyStartsNewLine(true);
    mExpressionText.setFont(
        juce::Font::fromString(juce::Font::getDefaultMonospacedFontName()).withHeight(14));
    addAndMakeVisible(mExpressionText);
}

ExpressionEditorComponent::~ExpressionEditorComponent()
{
}

void ExpressionEditorComponent::paint (juce::Graphics& g)
{
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
