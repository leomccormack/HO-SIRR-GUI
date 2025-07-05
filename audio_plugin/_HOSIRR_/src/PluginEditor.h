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

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "outputCoordsView.h"
#include "RIRview.h"
#include "../../resources/HOSIRRLookAndFeel.h"
#include <thread>

typedef enum _HOSIRR_WARNINGS{
    k_warning_none
}HOSIRR_WARNINGS;

class PluginEditor  : public AudioProcessorEditor,
                      public Timer,
                      private FilenameComponentListener,
                      public juce::ComboBox::Listener,
                      public juce::Slider::Listener,
                      public juce::Button::Listener
{
public:
    PluginEditor (PluginProcessor& p);
    ~PluginEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;

private:
    PluginProcessor& processor;
    void* hHS;
    void timerCallback() override;
#ifndef PLUGIN_EDITOR_DISABLE_OPENGL
    std::unique_ptr<OpenGLGraphicsContextCustomShader> shader;
    OpenGLContext openGLContext;
#endif
    int RenderingInProgress;
    double progress = 0.0;
    ProgressBar progressbar;

    /* Look and Feel */
    HOSIRRLookAndFeel LAF;

    /* source coordinates viewport */
    std::unique_ptr<Viewport> outputCoordsVP;
    outputCoordsView* outputCoordsView_handle;
    FilenameComponent fileChooser;

    /* json file loading/saving */
    std::unique_ptr<juce::FileChooser> chooser;

    /* wav file loading */
    AudioFormatManager formatManager;
    AudioSampleBuffer fileBuffer;
    float durationInSeconds;
    void filenameComponentChanged (FilenameComponent*) override  {
        String directory = fileChooser.getCurrentFile().getFullPathName();
        processor.setLoadWavDirectory(directory);
        loadWavFile();
        thumbnailComp->setFile(fileChooser.getCurrentFile());
    }
    void loadWavFile()
    {
        String directory = processor.getLoadWavDirectory();
        std::unique_ptr<AudioFormatReader> reader (formatManager.createReaderFor (directory));

        if (reader.get() != nullptr) { /* if file exists */
            durationInSeconds = (float)reader->lengthInSamples / (float)reader->sampleRate;

            if (reader->numChannels <= 1024 /* maximum number of channels for WAV files */) {
                fileBuffer.setSize ((int)reader->numChannels, (int) reader->lengthInSamples);
                reader->read (&fileBuffer, 0, (int) reader->lengthInSamples, 0, true, true);
            }
            const float* const* H = fileBuffer.getArrayOfReadPointers();
            hosirrlib_setAmbiRIR(hHS, H, fileBuffer.getNumChannels(), fileBuffer.getNumSamples(), (int)reader->sampleRate);

        }
    }

    /* RIR display */
    std::unique_ptr<Viewport> RIRviewVP;
    AudioThumbnailCache thumbnailCache;
    RIRview* thumbnailComp;
    float dispTimeTrim;
    float dispGain_dB;
    RIR_VIEW_WAV_LABELS currentView;

    /* warnings */
    HOSIRR_WARNINGS currentWarning;

    /* tooltips */
    SharedResourcePointer<TooltipWindow> tipWindow;
    std::unique_ptr<juce::ComboBox> pluginDescription; /* Dummy combo box to provide plugin description tooltip */
    HyperlinkButton publicationLink { "(Related Publication)", { "https://leomccormack.github.io/sparta-site/docs/help/related-publications/mccormack2020higher.pdf" } };

    std::unique_ptr<juce::ComboBox> CBoutputDirsPreset;
    std::unique_ptr<SliderWithAttachment> SL_num_loudspeakers;
    std::unique_ptr<ComboBoxWithAttachment> CBchFormat;
    std::unique_ptr<ComboBoxWithAttachment> CBnormScheme;
    std::unique_ptr<juce::TextButton> tb_loadJSON;
    std::unique_ptr<juce::TextButton> tb_saveJSON;
    std::unique_ptr<ComboBoxWithAttachment> CBanaOrder;
    std::unique_ptr<juce::TextButton> tb_render;
    std::unique_ptr<juce::TextButton> tb_saveRIR;
    std::unique_ptr<juce::Label> label_inputOrder;
    std::unique_ptr<juce::Label> label_inputLength;
    std::unique_ptr<juce::Label> label_inputSampleRate;
    std::unique_ptr<juce::Slider> SL_wetDryBalance;
    std::unique_ptr<juce::Slider> SL_windowSize;
    std::unique_ptr<juce::ComboBox> CBdisplayRIR;
    std::unique_ptr<juce::Slider> SL_displayGain;
    std::unique_ptr<juce::Slider> SL_displayTimeTrim;
    std::unique_ptr<juce::ToggleButton> tb_BroadBand1stPeak;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
