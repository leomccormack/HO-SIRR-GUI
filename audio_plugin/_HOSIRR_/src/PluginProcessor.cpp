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

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>("inputOrder", "InputOrder",
                                                                  juce::StringArray{"1st order","2nd order","3rd order","4th order","5th order","6th order","7th order"}, 0,
                                                                  AudioParameterChoiceAttributes().withAutomatable(false)));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("channelOrder", "ChannelOrder", juce::StringArray{"ACN", "FuMa"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("normType", "NormType", juce::StringArray{"N3D", "SN3D", "FuMa"}, 1));
    params.push_back(std::make_unique<juce::AudioParameterInt>("numLoudspeakers", "NumLoudspeakers", 4, HOSIRR_MAX_NUM_OUTPUTS, 1,
                                                               AudioParameterIntAttributes().withAutomatable(false)));
    for(int i=0; i<HOSIRR_MAX_NUM_OUTPUTS; i++){
        params.push_back(std::make_unique<juce::AudioParameterFloat>("azim" + juce::String(i), "Azim_" + juce::String(i+1), juce::NormalisableRange<float>(-180.0f, 180.0f, 0.01f), 0.0f, AudioParameterFloatAttributes().withAutomatable(false)));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("elev" + juce::String(i), "Elev_" + juce::String(i+1), juce::NormalisableRange<float>(-90.0f, 90.0f, 0.01f), 0.0f, AudioParameterFloatAttributes().withAutomatable(false)));
    }
    
    
    return { params.begin(), params.end() };
}

void PluginProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
   if (parameterID == "inputOrder"){
       hosirrlib_setAnalysisOrder(hHS, static_cast<int>(newValue+1.001f));
    }
    else if (parameterID == "channelOrder"){
        hosirrlib_setChOrder(hHS, static_cast<int>(newValue+1.001f));
    }
    else if (parameterID == "normType"){
        hosirrlib_setNormType(hHS, static_cast<int>(newValue+1.001f));
    }
    else if(parameterID == "numLoudspeakers"){
        hosirrlib_setNumLoudspeakers(hHS, static_cast<int>(newValue));
    }
    for(int i=0; i<HOSIRR_MAX_NUM_OUTPUTS; i++){
        if(parameterID == "azim" + juce::String(i)){
            hosirrlib_setLoudspeakerAzi_deg(hHS, i, newValue);
            break;
        }
        else if(parameterID == "elev" + juce::String(i)){
            hosirrlib_setLoudspeakerElev_deg(hHS, i, newValue);
            break;
        }
    }
}

void PluginProcessor::setParameterValuesUsingInternalState()
{
    setParameterValue("inputOrder", hosirrlib_getAnalysisOrder(hHS)-1);
    setParameterValue("channelOrder", hosirrlib_getChOrder(hHS)-1);
    setParameterValue("normType", hosirrlib_getNormType(hHS)-1);
    setParameterValue("numLoudspeakers", hosirrlib_getNumLoudspeakers(hHS));
    for(int i=0; i<HOSIRR_MAX_NUM_OUTPUTS; i++){
        setParameterValue("azim" + juce::String(i), hosirrlib_getLoudspeakerAzi_deg(hHS, i));
        setParameterValue("elev" + juce::String(i), hosirrlib_getLoudspeakerElev_deg(hHS, i));
    }
}

PluginProcessor::PluginProcessor() :
	AudioProcessor(BusesProperties()
		.withInput("Input", AudioChannelSet::discreteChannels(MAX_NUM_CHANNELS), true)
	    .withOutput("Output", AudioChannelSet::discreteChannels(MAX_NUM_CHANNELS), true)),
    ParameterManager(*this, createParameterLayout())
{
	hosirrlib_create(&hHS);
    
    /* Grab defaults */
    setParameterValuesUsingInternalState();
}

PluginProcessor::~PluginProcessor()
{
	hosirrlib_destroy(&hHS);
}

void PluginProcessor::setCurrentProgram (int /*index*/)
{
}

const String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 0;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

const String PluginProcessor::getProgramName (int /*index*/)
{
    return String();
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

void PluginProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    nHostBlockSize = samplesPerBlock;
    nNumInputs =  getTotalNumInputChannels();
    nNumOutputs = getTotalNumOutputChannels();
    nSampleRate = (int)(sampleRate + 0.5);
}

void PluginProcessor::releaseResources()
{
}

void PluginProcessor::processBlock (AudioSampleBuffer& /*buffer*/, MidiBuffer& /*midiMessages*/)
{
    
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; 
}

AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (MemoryBlock& destData)
{
    juce::ValueTree state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->setTagName("HOSIRRPLUGINSETTINGS");
    xml->setAttribute("VersionCode", JucePlugin_VersionCode); // added since 0x10006
    
    /* Now for the other DSP object parameters (that have no JUCE parameter counterpart) */
    
    /* Other */
    xml->setAttribute("JSONFilePath", lastJSONDir.getFullPathName());
    xml->setAttribute("LoadWavFilePath", getLoadWavDirectory());
    xml->setAttribute("SaveWavFilePath", lastSaveWavDirectory.getFullPathName());
    
    /* Save */
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    /* Load */
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName("HOSIRRPLUGINSETTINGS")){
        if(!xmlState->hasAttribute("VersionCode")){ // pre-0x10006
            if(xmlState->hasAttribute("AnalysisOrder"))
                hosirrlib_setAnalysisOrder(hHS, xmlState->getIntAttribute("AnalysisOrder",1));
            for(int i=0; i<hosirrlib_getMaxNumLoudspeakers(); i++){
                if(xmlState->hasAttribute("LoudspeakerAziDeg" + String(i)))
                    hosirrlib_setLoudspeakerAzi_deg(hHS, i, (float)xmlState->getDoubleAttribute("LoudspeakerAziDeg" + String(i), 0.0f));
                if(xmlState->hasAttribute("LoudspeakerElevDeg" + String(i)))
                    hosirrlib_setLoudspeakerElev_deg(hHS, i, (float)xmlState->getDoubleAttribute("LoudspeakerElevDeg" + String(i), 0.0f));
            }
            if(xmlState->hasAttribute("nLoudspeakers"))
                hosirrlib_setNumLoudspeakers(hHS, xmlState->getIntAttribute("nLoudspeakers", 1));
            if(xmlState->hasAttribute("Norm"))
                hosirrlib_setNormType(hHS, xmlState->getIntAttribute("Norm", 1));
            if(xmlState->hasAttribute("ChOrder"))
                hosirrlib_setChOrder(hHS, xmlState->getIntAttribute("ChOrder", 1));
            
            if(xmlState->hasAttribute("JSONFilePath"))
                lastJSONDir = xmlState->getStringAttribute("JSONFilePath", "");
            if(xmlState->hasAttribute("LoadWavFilePath"))
                setLoadWavDirectory(xmlState->getStringAttribute("LoadWavFilePath", ""));
            if(xmlState->hasAttribute("SaveWavFilePath"))
                setSaveWavDirectory(xmlState->getStringAttribute("SaveWavFilePath", ""));
            
            setParameterValuesUsingInternalState();
        }
        else if(xmlState->getIntAttribute("VersionCode")>=0x10006){
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
            
            /* Now for the other DSP object parameters (that have no JUCE parameter counterpart) */
            
            /* Other */
            if(xmlState->hasAttribute("JSONFilePath"))
                lastJSONDir = xmlState->getStringAttribute("JSONFilePath", "");
            if(xmlState->hasAttribute("LoadWavFilePath"))
                setLoadWavDirectory(xmlState->getStringAttribute("LoadWavFilePath", ""));
            if(xmlState->hasAttribute("SaveWavFilePath"))
                setSaveWavDirectory(xmlState->getStringAttribute("SaveWavFilePath", ""));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

/* Adapted from the AllRADecoder by Daniel Rudrich, (c) 2017 (GPLv3 license) */
void PluginProcessor::saveConfigurationToFile (File destination)
{
    loudspeakers.removeAllChildren(nullptr);
    for (int i=0; i<hosirrlib_getNumLoudspeakers(hHS);i++)
    {
        loudspeakers.appendChild (ConfigurationHelper::
                                  createElement(hosirrlib_getLoudspeakerAzi_deg(hHS, i),
                                                hosirrlib_getLoudspeakerElev_deg(hHS, i),
                                                1.0f,
                                                i+1,
                                                false,
                                                1.0f), nullptr);
    }
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("Name", var("HOSIRR loudspeaker directions."));
    char versionString[10];
    strcpy(versionString, "v");
    strcat(versionString, JucePlugin_VersionString);
    jsonObj->setProperty("Description", var("This configuration file was created with the HOSIRR " + String(versionString) + " plug-in. " + Time::getCurrentTime().toString(true, true)));
    jsonObj->setProperty ("LoudspeakerLayout", ConfigurationHelper::convertLoudspeakersToVar (loudspeakers, "Loudspeaker Directions"));
    Result result = ConfigurationHelper::writeConfigurationToFile (destination, var (jsonObj));
}

/* Adapted from the AllRADecoder by Daniel Rudrich, (c) 2017 (GPLv3 license) */
void PluginProcessor::loadConfiguration (const File& configFile)
{
    int channelIDs[MAX_NUM_CHANNELS+1] = {0};
    int virtual_channelIDs[MAX_NUM_CHANNELS+1] = {0};
    loudspeakers.removeAllChildren(nullptr);
    Result result = ConfigurationHelper::parseFileForLoudspeakerLayout (configFile, loudspeakers, nullptr);
    if(result.wasOk()){
        int num_ls, num_virtual_ls, ls_idx, jj;
        num_ls = num_virtual_ls = ls_idx = jj = 0;
        /* get Channel IDs and find number of directions and virtual directions */
        for (ValueTree::Iterator it = loudspeakers.begin(); it != loudspeakers.end(); ++it){
            if ( !((*it).getProperty("Imaginary"))){
                num_ls++; channelIDs[jj] = (*it).getProperty("Channel");
            }
            else{
                virtual_channelIDs[num_virtual_ls] = (*it).getProperty("Channel");
                num_virtual_ls++; channelIDs[jj] = -1;
            }
            jj++;
        }
        /* remove virtual channels and shift the channel indices down */
        for(int i=0; i<num_virtual_ls; i++)
            for(int j=0; j<num_ls+num_virtual_ls; j++)
                if(channelIDs[j] == -1)
                    for(int k=j; k<num_ls+num_virtual_ls; k++)
                        channelIDs[k] = channelIDs[k+1];
        
        /* then decriment the channel IDs to remove the gaps */
        for(int i=0; i<num_virtual_ls; i++)
            for(int j=0; j<num_ls+num_virtual_ls; j++)
                if( channelIDs[j] > virtual_channelIDs[i]-i )
                    channelIDs[j]--;
        
        /* update with the new configuration  */
        hosirrlib_setNumLoudspeakers(hHS, num_ls);
        for (ValueTree::Iterator it = loudspeakers.begin() ; it != loudspeakers.end(); ++it){
            if ( !((*it).getProperty("Imaginary"))){
                hosirrlib_setLoudspeakerAzi_deg(hHS, channelIDs[ls_idx]-1, (*it).getProperty("Azimuth"));
                hosirrlib_setLoudspeakerElev_deg(hHS, channelIDs[ls_idx]-1, (*it).getProperty("Elevation"));
                ls_idx++;
            }
        }
    } 
}
