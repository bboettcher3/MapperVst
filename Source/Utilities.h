/*
  ==============================================================================

    Utilities.h
    Created: 22 Jul 2022 9:24:42am
    Author:  brady

  ==============================================================================
*/

#pragma once

/*
  ==============================================================================

    Utils.h
    Created: 10 Apr 2021 3:27:51pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <chrono>

namespace Utilities {

static constexpr int NUM_COLOURS = 12;

/* Returns one of the 12 predefined colours distributed across the colour wheel */
static inline juce::Colour getRainbow12Colour(int value) {
  jassert(value >= 0 && value <= 11);
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  switch (value) {
    case 0:  // red
      r = 1.0f;
      g = 0.0f;
      b = 0.0f;
      break;
    case 1:  // orange
      r = 1.0f;
      g = 0.5f;
      b = 0.0f;
      break;
    case 2:  // yellow
      r = 1.0f;
      g = 1.0f;
      b = 0.0f;
      break;
    case 3:  // chartreuse
      r = 0.5f;
      g = 1.0f;
      b = 0.0f;
      break;
    case 4:  // green
      r = 0.0f;
      g = 1.0f;
      b = 0.0f;
      break;
    case 5:  // spring green
      r = 0.0f;
      g = 1.0f;
      b = 0.5f;
      break;
    case 6:  // cyan
      r = 0.0f;
      g = 1.0f;
      b = 1.0f;
      break;
    case 7:  // dodger blue
      r = 0.0f;
      g = 0.5f;
      b = 1.0f;
      break;
    case 8:  // blue
      r = 0.0f;
      g = 0.0f;
      b = 1.0f;
      break;
    case 9:  // purple
      r = 0.5f;
      g = 0.0f;
      b = 1.0f;
      break;
    case 10:  // violet
      r = 1.0f;
      g = 0.0f;
      b = 1.0f;
      break;
    case 11:  // magenta
      r = 1.0f;
      g = 0.0f;
      b = 0.5f;
      break;
    default:
      break;
  }
  return juce::Colour(r * 255.0f, g * 255.0f, b * 255.0f);
}
};  // namespace Utilities
