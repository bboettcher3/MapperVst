/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MapperVstAudioProcessorEditor::MapperVstAudioProcessorEditor (MapperVstAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    mGraph = mpr_graph_new(MPR_OBJ);
    //mDummyDevice = mpr_dev_new(strdup("dummybitch"), mGraph);
    mpr_graph_add_cb(mGraph, graphCallbackHandler, MPR_OBJ, nullptr);
    startTimer(1000);

    setSize(400, 300);
}

MapperVstAudioProcessorEditor::~MapperVstAudioProcessorEditor()
{
    //if (mDummyDevice) mpr_dev_free(mDummyDevice);
    if (mGraph) mpr_graph_free(mGraph);
}

void MapperVstAudioProcessorEditor::timerCallback() {
    // Print graph info
    // interface
    juce::String intf = juce::String(mpr_graph_get_interface(mGraph));
    // IP
    juce::String ip = juce::String(mpr_graph_get_address(mGraph));
    DBG("intf: " << intf << ", ip: " << ip << "\n");

    // Print devices
    mpr_list devs = mpr_graph_get_list(mGraph, MPR_DEV);
    while (devs) {
        mpr_dev dev = *devs;
        juce::String devName = juce::String(mpr_obj_get_prop_as_str(dev, MPR_PROP_NAME, NULL));
        juce::String devicesString = juce::String("device ");
        devicesString.append(devName, devName.length());
        // Make list of signals for each device
        mpr_list sigs = mpr_dev_get_sigs(dev, MPR_DIR_ANY);
        while (sigs) {
            mpr_sig sig = *sigs;
            juce::String sigName = juce::String(mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, NULL));
            devicesString.append(sigName, sigName.length());
            devicesString.append(" ", 1);
            sigs = mpr_list_get_next(sigs);
        }
        DBG(devicesString << "\n");
        devs = mpr_list_get_next(devs);
    }
    DBG("done printing devices \n");
}

void MapperVstAudioProcessorEditor::graphCallbackHandler(mpr_graph g, mpr_obj o, mpr_graph_evt e, const void* v)
{
    DBG("called back\n");
}

//==============================================================================
void MapperVstAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MapperVstAudioProcessorEditor::resized()
{

}
