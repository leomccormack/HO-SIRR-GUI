/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.3

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "outputCoordsView.h"
#include "RIRview.h"
#include "../../resources/HOSIRRLookAndFeel.h"
#include <thread>

typedef enum _HOSIRR_WARNINGS{
    k_warning_none
}HOSIRR_WARNINGS;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PluginEditor  : public AudioProcessorEditor,
                      public Timer,
                      private FilenameComponentListener,
                      public juce::ComboBox::Listener,
                      public juce::Slider::Listener,
                      public juce::Button::Listener
{
public:
    //==============================================================================
    PluginEditor (PluginProcessor* ownerFilter);
    ~PluginEditor() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.


    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    PluginProcessor* hVst;
    void* hHS;
    void timerCallback() override;
    std::unique_ptr<OpenGLGraphicsContextCustomShader> shader;
    OpenGLContext openGLContext;
    int RenderingInProgress;
    double progress = 0.0;
    ProgressBar progressbar;

    /* Look and Feel */
    HOSIRRLookAndFeel LAF;

    /* source coordinates viewport */
    std::unique_ptr<Viewport> outputCoordsVP;
    outputCoordsView* outputCoordsView_handle;
    FilenameComponent fileChooser;

    /* wav file loading */
    AudioFormatManager formatManager;
    AudioSampleBuffer fileBuffer;
    float durationInSeconds;
    void filenameComponentChanged (FilenameComponent*) override  {
        String directory = fileChooser.getCurrentFile().getFullPathName();
        hVst->setLoadWavDirectory(directory);
        loadWavFile();
        thumbnailComp->setFile(fileChooser.getCurrentFile());
    }
    void loadWavFile()
    {
        String directory = hVst->getLoadWavDirectory();
        std::unique_ptr<AudioFormatReader> reader (formatManager.createReaderFor (directory));

        if (reader.get() != nullptr) { /* if file exists */
            durationInSeconds = (float)reader->lengthInSamples / (float)reader->sampleRate;

            if (reader->numChannels <= 1024 /* maximum number of channels for WAV files */) {
                fileBuffer.setSize ((int)reader->numChannels, (int) reader->lengthInSamples);
                reader->read (&fileBuffer, 0, (int) reader->lengthInSamples, 0, true, true);
            }
            const float** H = fileBuffer.getArrayOfReadPointers();
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

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::ComboBox> CBoutputDirsPreset;
    std::unique_ptr<juce::Slider> SL_num_loudspeakers;
    std::unique_ptr<juce::ComboBox> CBchFormat;
    std::unique_ptr<juce::ComboBox> CBnormScheme;
    std::unique_ptr<juce::TextButton> tb_loadJSON;
    std::unique_ptr<juce::TextButton> tb_saveJSON;
    std::unique_ptr<juce::ComboBox> CBanaOrder;
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


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

