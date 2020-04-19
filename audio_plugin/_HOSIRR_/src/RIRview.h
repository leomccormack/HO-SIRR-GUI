/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.4

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

typedef enum _RIR_VIEW_WAV_LABELS{
    RIR_VIEW_SH_LABELS = 1,
    RIR_VIEW_LS_LABELS

}RIR_VIEW_WAV_LABELS;

const String SHLabels[64] = {"Y00", "Y1(-1)", "Y10", "Y11", "Y2(-2)", "Y2(-1)", "Y20", "Y21", "Y22",
    "Y3(-3)", "Y3(-2)", "Y3(-1)", "Y30", "Y31", "Y32", "Y33", "Y4(-4)", "Y4(-3)",
    "Y4(-2)", "Y4(-1)", "Y40", "Y41", "Y42", "Y43", "Y44", "Y5(-5)", "Y5(-4)",
    "Y5(-3)", "Y5(-2)", "Y5(-1)", "Y50", "Y51", "Y52", "Y53", "Y54", "Y55",
    "Y6(-6)", "Y6(-5)", "Y6(-4)", "Y6(-3)", "Y6(-2)", "Y6(-1)", "Y60", "Y61", "Y62",
    "Y63", "Y64", "Y65", "Y66", "Y7(-7)", "Y7(-6)", "Y7(-5)", "Y7(-4)", "Y7(-3)",
    "Y7(-2)", "Y7(-1)", "Y70", "Y71", "Y72", "Y73", "Y74", "Y75", "Y76","Y77"
};

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class RIRview  : public Component,
                 private ChangeListener
{
public:
    //==============================================================================
    RIRview (int sourceSamplesPerThumbnailSample, AudioFormatManager& formatManager, AudioThumbnailCache& cache, int width, int height);
    ~RIRview();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

    void setFile (const File& file)
    {
        thumbnail.setSource (new FileInputSource (file));
    }

    void paintIfNoFileLoaded (Graphics& g)
    {
        setBounds(0,0,compWidth, compHeight);
        //g.fillAll (Colours::black);
        g.fillAll (Colour (0xff323e44));
        g.setColour (Colours::white);
        g.drawFittedText ("No RIR Loaded", getLocalBounds(), Justification::centred, 1);
    }

    void paintIfFileLoaded (Graphics& g)
    {
        /* scale bounds based on number of channels */
        int nCh = thumbnail.getNumChannels();
        int waveformHeight = compHeight;
        if(getHeight()!=nCh*waveformHeight)
            setBounds(0,0,compWidth, nCh*waveformHeight);

        /* draw waveforms */
        //g.fillAll (Colours::black);
        g.fillAll (Colour (0xff323e44));
        g.setColour (Colours::white);
        thumbnail.drawChannels (g, getLocalBounds(), 0.0, timeTrim*thumbnail.getTotalLength(), gain_lin);

        /* draw lines and labels for each channel */
        for(int i=0; i<nCh; i++){
            g.setColour (Colours::black);
            g.drawLine(0, (i+1)*waveformHeight, compWidth, (i+1)*waveformHeight, 1);
            g.setColour (Colours::lightgrey);
            switch(labels){
                case RIR_VIEW_SH_LABELS:
                    g.drawText(SHLabels[i], compWidth/2-40, i*waveformHeight + 4, 80, 10, Justification::centred);
                    break;
                case RIR_VIEW_LS_LABELS:
                    g.drawText("#"+String(i+1), compWidth/2-40, i*waveformHeight + 4, 80, 10, Justification::centred);
                    break;
            }
        }
    }
    
    void setLabelsView(RIR_VIEW_WAV_LABELS labelview)
    {
        labels = labelview;
        thumbnailChanged();
    }
    
    RIR_VIEW_WAV_LABELS getLabelsView()
    {
        return labels;
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            thumbnailChanged();
    }

    void setTimeTrim(float newValue){
        timeTrim = newValue;
        thumbnailChanged();
    }

    void setGain_dB(float newValue){
        gain_dB = newValue;
        gain_lin = powf(10.0f, gain_dB/20.0f);
        thumbnailChanged();
    }

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    void thumbnailChanged()
    {
        repaint();
    }

    AudioThumbnail thumbnail;
    int compHeight, compWidth;
    RIR_VIEW_WAV_LABELS labels;
    float timeTrim, gain_dB, gain_lin;
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RIRview)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

