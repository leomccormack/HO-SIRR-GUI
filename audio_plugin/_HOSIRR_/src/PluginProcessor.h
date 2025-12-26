/*
 ==============================================================================
 
 This file is part of HOSIRR
 Copyright (c) 2020 - Leo McCormack
 
 HOSIRR is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 HOSIRR is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with HOSIRR.  If not, see <http://www.gnu.org/licenses/>.
 
 ==============================================================================
 */

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include <JuceHeader.h>
#include "../../resources/PluginProcessorBase.h"
#include "hosirrlib.h"
#include <atomic>

#define CONFIGURATIONHELPER_ENABLE_LOUDSPEAKERLAYOUT_METHODS 1
#define CONFIGURATIONHELPER_ENABLE_GENERICLAYOUT_METHODS 1
#include "../../resources/ConfigurationHelper.h"
#define BUILD_VER_SUFFIX "" /* String to be adsded before the version name on the GUI (e.g. beta, alpha etc..) */
#define MAX_NUM_CHANNELS 64

class PluginProcessor  : public PluginProcessorBase
{
public:
    PluginProcessor();
    ~PluginProcessor();

    /* PluginProcessorBase mandatory overrides */
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {};
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    
    /* PluginEditor */
    juce::AudioProcessorEditor* createEditor() override;
    
    /* Get functions */
    void* getFXHandle() { return hHS; }
    
    /* JSON */
    void saveConfigurationToFile (File destination);
    void loadConfiguration (const File& presetFile);
    void setLastJSONDir(File newLastDir){ lastJSONDir = newLastDir; }
    File getLastJSONDir() {return lastJSONDir;}

    /* last save/load directories */
    void setLoadWavDirectory(String newDirectory){ lastLoadWavDirectory = newDirectory; }
    String getLoadWavDirectory(){ return lastLoadWavDirectory; }
    void setSaveWavDirectory(File newDirectory){ lastSaveWavDirectory = newDirectory; }
    File getSaveWavDirectory(){ return lastSaveWavDirectory; }
    
private:
    void* hHS;                         /* hosirrlib handle */
    File lastJSONDir;
    ValueTree loudspeakers {"Loudspeakers"};
    String lastLoadWavDirectory;
    File lastSaveWavDirectory;
    
    /* For syncing parameter values between the JUCE parameter tree and the internal DSP object */
    void setParameterValuesUsingInternalState();
    void setInternalStateUsingParameterValues();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
