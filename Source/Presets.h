/*
  ==============================================================================

    Presets.h
    Created: 3 Aug 2022 6:18:27pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "MapperManager.h"

namespace Preset {
// Saves the current mappings
static void save(juce::File file, MapperManager::Map** maps, int numMaps) {

}

// Opens a mapping file saved with this application
static void open(juce::File file, MapperManager::Map** maps, int numMaps) {}

};  // namespace Presets