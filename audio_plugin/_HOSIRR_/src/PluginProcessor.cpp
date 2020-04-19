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

PluginProcessor::PluginProcessor() : 
	AudioProcessor(BusesProperties()
		.withInput("Input", AudioChannelSet::discreteChannels(64), true)
	    .withOutput("Output", AudioChannelSet::discreteChannels(64), true))
{
	hosirrlib_create(&hHS);
}

PluginProcessor::~PluginProcessor()
{
	hosirrlib_destroy(&hHS);
}

void PluginProcessor::setParameter (int index, float newValue)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_analysisOrder:   hosirrlib_setAnalysisOrder(hHS, (ANALYSIS_ORDERS)(int)(newValue*(float)(HOSIRR_MAX_SH_ORDER-1) + 1.5f)); break;
            case k_channelOrder:    hosirrlib_setChOrder(hHS, (int)(newValue*(float)(HOSIRR_NUM_CH_ORDERINGS-1) + 1.5f)); break;
            case k_normType:        hosirrlib_setNormType(hHS, (int)(newValue*(float)(HOSIRR_NUM_NORM_TYPES-1) + 1.5f)); break;
            case k_numLoudspeakers: hosirrlib_setNumLoudspeakers(hHS, (int)(newValue*(float)(HOSIRR_MAX_NUM_OUTPUTS)+0.5)); break;
        }
    }
    /* loudspeaker direction parameters */
    else{
        index-=k_NumOfParameters;
        float newValueScaled;
        if (!(index % 2)){
            newValueScaled = (newValue - 0.5f)*360.0f;
            if (newValueScaled != hosirrlib_getLoudspeakerAzi_deg(hHS, index/2))
                hosirrlib_setLoudspeakerAzi_deg(hHS, index/2, newValueScaled);
        }
        else{
            newValueScaled = (newValue - 0.5f)*180.0f;
            if (newValueScaled != hosirrlib_getLoudspeakerElev_deg(hHS, index/2))
                hosirrlib_setLoudspeakerElev_deg(hHS, index/2, newValueScaled);
        }
    }
}

void PluginProcessor::setCurrentProgram (int /*index*/)
{
}

float PluginProcessor::getParameter (int index)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_analysisOrder:   return (float)(hosirrlib_getAnalysisOrder(hHS)-1)/(float)(HOSIRR_MAX_SH_ORDER-1);
            case k_channelOrder:    return (float)(hosirrlib_getChOrder(hHS)-1)/(float)(HOSIRR_NUM_CH_ORDERINGS-1);
            case k_normType:        return (float)(hosirrlib_getNormType(hHS)-1)/(float)(HOSIRR_NUM_NORM_TYPES-1);
            case k_numLoudspeakers: return (float)(hosirrlib_getNumLoudspeakers(hHS))/(float)(HOSIRR_MAX_NUM_OUTPUTS);
            default: return 0.0f;
        }
    }
    /* loudspeaker direction parameters */
    else{
        index-=k_NumOfParameters;
        if (!(index % 2))
            return (hosirrlib_getLoudspeakerAzi_deg(hHS, index/2)/360.0f) + 0.5f;
        else
            return (hosirrlib_getLoudspeakerElev_deg(hHS, (index-1)/2)/180.0f) + 0.5f;
    }
}

int PluginProcessor::getNumParameters()
{
	return k_NumOfParameters + 2*HOSIRR_MAX_NUM_OUTPUTS;
}

const String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

const String PluginProcessor::getParameterName (int index)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_analysisOrder:   return "order";
            case k_channelOrder:    return "channel_order";
            case k_normType:        return "norm_type";
            case k_numLoudspeakers: return "num_loudspeakers";
            default: return "NULL";
        }
    }
    /* loudspeaker direction parameters */
    else{
        index-=k_NumOfParameters;
        if (!(index % 2))
            return TRANS("Azim_") + String(index/2);
        else
            return TRANS("Elev_") + String((index-1)/2);
    }
}

const String PluginProcessor::getParameterText(int index)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_analysisOrder: return String(hosirrlib_getAnalysisOrder(hHS));
            case k_channelOrder:
                switch(hosirrlib_getChOrder(hHS)){
                    case CH_ACN:  return "ACN";
                    case CH_FUMA: return "FuMa";
                    default: return "NULL";
                }
            case k_normType:
                switch(hosirrlib_getNormType(hHS)){
                    case NORM_N3D:  return "N3D";
                    case NORM_SN3D: return "SN3D";
                    case NORM_FUMA: return "FuMa";
                    default: return "NULL";
                }
                
            case k_numLoudspeakers:  return String(hosirrlib_getNumLoudspeakers(hHS));
        
            default: return "NULL";
        }
    }
    /* loudspeaker direction parameters */
    else{
        index-=k_NumOfParameters;
        if (!(index % 2))
            return String(hosirrlib_getLoudspeakerAzi_deg(hHS, index/2));
        else
            return String(hosirrlib_getLoudspeakerElev_deg(hHS, (index-1)/2));
    }
}

const String PluginProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String PluginProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
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


bool PluginProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool PluginProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
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

bool PluginProcessor::silenceInProducesSilenceOut() const
{
    return false;
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
    return new PluginEditor (this);
}

//==============================================================================
void PluginProcessor::getStateInformation (MemoryBlock& destData)
{
    XmlElement xml("HOSIRRPLUGINSETTINGS");
 
    xml.setAttribute("AnalysisOrder", hosirrlib_getAnalysisOrder(hHS));
    for(int i=0; i<hosirrlib_getMaxNumLoudspeakers(); i++){
        xml.setAttribute("LoudspeakerAziDeg" + String(i), hosirrlib_getLoudspeakerAzi_deg(hHS,i));
        xml.setAttribute("LoudspeakerElevDeg" + String(i), hosirrlib_getLoudspeakerElev_deg(hHS,i));
    }
    xml.setAttribute("nLoudspeakers", hosirrlib_getNumLoudspeakers(hHS));
    xml.setAttribute("Norm", hosirrlib_getNormType(hHS));
    xml.setAttribute("ChOrder", hosirrlib_getChOrder(hHS));
    
    xml.setAttribute("JSONFilePath", lastJSONDir.getFullPathName());
    xml.setAttribute("LoadWavFilePath", getLoadWavDirectory());
    xml.setAttribute("SaveWavFilePath", lastSaveWavDirectory.getFullPathName());
    
    //if(!ambi_dec_getUseDefaultHRIRsflag(hAmbi))
    //    xml.setAttribute("SofaFilePath", String(ambi_dec_getSofaFilePath(hAmbi)));

    copyXmlToBinary(xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    int i;

    if (xmlState != nullptr) {
        if (xmlState->hasTagName("HOSIRRPLUGINSETTINGS")) {
            if(xmlState->hasAttribute("AnalysisOrder"))
                hosirrlib_setAnalysisOrder(hHS, xmlState->getIntAttribute("AnalysisOrder",1));
            for(i=0; i<hosirrlib_getMaxNumLoudspeakers(); i++){
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
        int num_ls, num_virtual_ls, ls_idx, j;
        num_ls = num_virtual_ls = ls_idx = j = 0;
        /* get Channel IDs and find number of directions and virtual directions */
        for (ValueTree::Iterator it = loudspeakers.begin(); it != loudspeakers.end(); ++it){
            if ( !((*it).getProperty("Imaginary"))){
                num_ls++; channelIDs[j] = (*it).getProperty("Channel");
            }
            else{
                virtual_channelIDs[num_virtual_ls] = (*it).getProperty("Channel");
                num_virtual_ls++; channelIDs[j] = -1;
            }
            j++;
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
