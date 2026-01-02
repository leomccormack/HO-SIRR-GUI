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

#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor(p), processor(p), progressbar(progress), fileChooser ("File", File(), true, false, false, "*.wav", String(),
      "Load *.wav File"), thumbnailCache (5)
{
    CBoutputDirsPreset.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (CBoutputDirsPreset.get());
    CBoutputDirsPreset->setEditableText (false);
    CBoutputDirsPreset->setJustificationType (juce::Justification::centredLeft);
    CBoutputDirsPreset->setTextWhenNothingSelected (TRANS("Default"));
    CBoutputDirsPreset->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    CBoutputDirsPreset->addItem (TRANS("Default"), 1);
    CBoutputDirsPreset->addListener (this);
    CBoutputDirsPreset->setBounds (520, 96, 112, 20);

    SL_num_loudspeakers = std::make_unique<SliderWithAttachment>(p.parameters, "numLoudspeakers");
    addAndMakeVisible (SL_num_loudspeakers.get());
    SL_num_loudspeakers->setSliderStyle (juce::Slider::LinearHorizontal);
    SL_num_loudspeakers->setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    SL_num_loudspeakers->addListener (this);
    SL_num_loudspeakers->setBounds (592, 124, 40, 20);

    CBchFormat = std::make_unique<ComboBoxWithAttachment>(p.parameters, "channelOrder");
    addAndMakeVisible (CBchFormat.get());
    CBchFormat->setEditableText (false);
    CBchFormat->setJustificationType (juce::Justification::centredLeft);
    CBchFormat->addListener (this);
    CBchFormat->setBounds (77, 95, 68, 20);

    CBnormScheme = std::make_unique<ComboBoxWithAttachment>(p.parameters, "normType");
    addAndMakeVisible (CBnormScheme.get());
    CBnormScheme->setEditableText (false);
    CBnormScheme->setJustificationType (juce::Justification::centredLeft);
    CBnormScheme->addListener (this);
    CBnormScheme->setBounds (147, 95, 72, 20);

    tb_loadJSON.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (tb_loadJSON.get());
    tb_loadJSON->setButtonText (TRANS("Import"));
    tb_loadJSON->setConnectedEdges (juce::Button::ConnectedOnRight);
    tb_loadJSON->addListener (this);
    tb_loadJSON->setBounds (456, 161, 34, 14);

    tb_saveJSON.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (tb_saveJSON.get());
    tb_saveJSON->setButtonText (TRANS("Export"));
    tb_saveJSON->setConnectedEdges (juce::Button::ConnectedOnLeft);
    tb_saveJSON->addListener (this);
    tb_saveJSON->setBounds (600, 161, 34, 14);

    CBanaOrder = std::make_unique<ComboBoxWithAttachment>(p.parameters, "inputOrder");
    addAndMakeVisible (CBanaOrder.get());
    CBanaOrder->setEditableText (false);
    CBanaOrder->setJustificationType (juce::Justification::centredLeft);
    CBanaOrder->addListener (this);
    CBanaOrder->setBounds (344, 95, 88, 20);

    tb_render.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (tb_render.get());
    tb_render->setButtonText (TRANS("Render"));
    tb_render->addListener (this);
    tb_render->setBounds (264, 65, 144, 18);

    tb_saveRIR.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (tb_saveRIR.get());
    tb_saveRIR->setButtonText (TRANS("Save"));
    tb_saveRIR->addListener (this);
    tb_saveRIR->setBounds (480, 65, 136, 18);

    label_inputOrder.reset (new juce::Label ("new label", juce::String()));
    addAndMakeVisible (label_inputOrder.get());
    label_inputOrder->setFont (juce::FontOptions (15.00f, juce::Font::plain).withStyle ("Regular"));
    label_inputOrder->setJustificationType (juce::Justification::centredLeft);
    label_inputOrder->setEditable (false, false, false);
    label_inputOrder->setBounds (130, 128, 88, 20);

    label_inputLength.reset (new juce::Label ("new label", juce::String()));
    addAndMakeVisible (label_inputLength.get());
    label_inputLength->setFont (juce::FontOptions (15.00f, juce::Font::plain).withStyle ("Regular"));
    label_inputLength->setJustificationType (juce::Justification::centredLeft);
    label_inputLength->setEditable (false, false, false);
    label_inputLength->setBounds (130, 152, 88, 20);

    label_inputSampleRate.reset (new juce::Label ("new label", juce::String()));
    addAndMakeVisible (label_inputSampleRate.get());
    label_inputSampleRate->setFont (juce::FontOptions (15.00f, juce::Font::plain).withStyle ("Regular"));
    label_inputSampleRate->setJustificationType (juce::Justification::centredLeft);
    label_inputSampleRate->setEditable (false, false, false);
    label_inputSampleRate->setBounds (130, 176, 88, 20);

    SL_wetDryBalance.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (SL_wetDryBalance.get());
    SL_wetDryBalance->setRange (0, 2, 0.01);
    SL_wetDryBalance->setSliderStyle (juce::Slider::LinearHorizontal);
    SL_wetDryBalance->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    SL_wetDryBalance->addListener (this);
    SL_wetDryBalance->setBounds (312, 151, 120, 20);

    SL_windowSize.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (SL_windowSize.get());
    SL_windowSize->setRange (32, 256, 2);
    SL_windowSize->setSliderStyle (juce::Slider::LinearHorizontal);
    SL_windowSize->setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    SL_windowSize->addListener (this);
    SL_windowSize->setBounds (360, 122, 71, 20);

    CBdisplayRIR.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (CBdisplayRIR.get());
    CBdisplayRIR->setEditableText (false);
    CBdisplayRIR->setJustificationType (juce::Justification::centredLeft);
    CBdisplayRIR->setTextWhenNothingSelected (TRANS("Ambi RIR"));
    CBdisplayRIR->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    CBdisplayRIR->addListener (this);
    CBdisplayRIR->setBounds (21, 229, 99, 18);

    SL_displayGain.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (SL_displayGain.get());
    SL_displayGain->setRange (-24, 24, 0.01);
    SL_displayGain->setSliderStyle (juce::Slider::LinearHorizontal);
    SL_displayGain->setTextBoxStyle (juce::Slider::NoTextBox, false, 50, 20);
    SL_displayGain->addListener (this);
    SL_displayGain->setBounds (344, 229, 88, 20);

    SL_displayTimeTrim.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (SL_displayTimeTrim.get());
    SL_displayTimeTrim->setRange (0.01, 1, 0.01);
    SL_displayTimeTrim->setSliderStyle (juce::Slider::LinearHorizontal);
    SL_displayTimeTrim->setTextBoxStyle (juce::Slider::NoTextBox, false, 50, 20);
    SL_displayTimeTrim->addListener (this);
    SL_displayTimeTrim->setBounds (208, 229, 88, 20);

    tb_BroadBand1stPeak.reset (new juce::ToggleButton ("new toggle button"));
    addAndMakeVisible (tb_BroadBand1stPeak.get());
    tb_BroadBand1stPeak->setButtonText (juce::String());
    tb_BroadBand1stPeak->addListener (this);
    tb_BroadBand1stPeak->setBounds (410, 174, 26, 24);

    setSize (656, 380);

    /* handles */
    hHS = processor.getFXHandle();

    /* init OpenGL */
#ifndef PLUGIN_EDITOR_DISABLE_OPENGL
    openGLContext.setMultisamplingEnabled(true);
    openGLContext.attachTo(*this);
#endif

    /* Look and Feel */
    setLookAndFeel(&LAF);

    /* remove slider bit of these sliders */
    SL_num_loudspeakers->setColour(Slider::trackColourId, Colours::transparentBlack);
    SL_num_loudspeakers->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    SL_num_loudspeakers->setSliderSnapsToMousePosition(false);
    SL_windowSize->setColour(Slider::trackColourId, Colours::transparentBlack);
    SL_windowSize->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    SL_windowSize->setSliderSnapsToMousePosition(false);

    /* add loudspeaker preset options */
    if(processor.wrapperType == AudioProcessor::wrapperType_AAX){
//        CBoutputDirsPreset->addItem (TRANS("ProTools Stereo"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_STEREO);
//        CBoutputDirsPreset->addItem (TRANS("ProTools LCR"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_LCR);
//        CBoutputDirsPreset->addItem (TRANS("ProTools Quad"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_QUAD);
        CBoutputDirsPreset->addItem (TRANS("ProTools 5.0"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_5_0);
        CBoutputDirsPreset->addItem (TRANS("ProTools 5.0.2"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_5_0_2);
        CBoutputDirsPreset->addItem (TRANS("ProTools 5.0.4"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_5_0_4);
        CBoutputDirsPreset->addItem (TRANS("ProTools 7.0"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_7_0);
        CBoutputDirsPreset->addItem (TRANS("ProTools 7.0.2"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_7_0_2);
        CBoutputDirsPreset->addItem (TRANS("ProTools 7.0.4"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_7_0_4);
        CBoutputDirsPreset->addItem (TRANS("ProTools 7.0.6"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_7_0_6);
        CBoutputDirsPreset->addItem (TRANS("ProTools 9.0.4"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_9_0_4);
        CBoutputDirsPreset->addItem (TRANS("ProTools 9.0.6"), LOUDSPEAKER_ARRAY_PRESET_PROTOOLS_9_0_6);
    }
    CBoutputDirsPreset->addItem (TRANS("5.x"), LOUDSPEAKER_ARRAY_PRESET_5PX);
    CBoutputDirsPreset->addItem (TRANS("7.x"), LOUDSPEAKER_ARRAY_PRESET_7PX);
    CBoutputDirsPreset->addItem (TRANS("8.x"), LOUDSPEAKER_ARRAY_PRESET_8PX);
    CBoutputDirsPreset->addItem (TRANS("9.x"), LOUDSPEAKER_ARRAY_PRESET_9PX);
    CBoutputDirsPreset->addItem (TRANS("10.x"), LOUDSPEAKER_ARRAY_PRESET_10PX);
    CBoutputDirsPreset->addItem (TRANS("11.x"), LOUDSPEAKER_ARRAY_PRESET_11PX);
    CBoutputDirsPreset->addItem (TRANS("11.x (7+4)"), LOUDSPEAKER_ARRAY_PRESET_11PX_7_4);
    CBoutputDirsPreset->addItem (TRANS("13.x"), LOUDSPEAKER_ARRAY_PRESET_13PX);
    CBoutputDirsPreset->addItem (TRANS("22.x"), LOUDSPEAKER_ARRAY_PRESET_22PX);
    CBoutputDirsPreset->addItem (TRANS("Aalto MCC"), LOUDSPEAKER_ARRAY_PRESET_AALTO_MCC);
    CBoutputDirsPreset->addItem (TRANS("Aalto MCC-subset"), LOUDSPEAKER_ARRAY_PRESET_AALTO_MCC_SUBSET);
    CBoutputDirsPreset->addItem (TRANS("Aalto Apaja"), LOUDSPEAKER_ARRAY_PRESET_AALTO_APAJA);
    CBoutputDirsPreset->addItem (TRANS("Aalto LR"), LOUDSPEAKER_ARRAY_PRESET_AALTO_LR);
    CBoutputDirsPreset->addItem (TRANS("DTU AVIL"), LOUDSPEAKER_ARRAY_PRESET_DTU_AVIL);
    CBoutputDirsPreset->addItem (TRANS("Zylia Lab (22.x)"), LOUDSPEAKER_ARRAY_PRESET_ZYLIA_LAB);
    CBoutputDirsPreset->addItem (TRANS("T-design (4)"), LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_4);
    CBoutputDirsPreset->addItem (TRANS("T-design (12)"), LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_12);
    CBoutputDirsPreset->addItem (TRANS("T-design (24)"), LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_24);
    CBoutputDirsPreset->addItem (TRANS("T-design (36)"), LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_36);
    CBoutputDirsPreset->addItem (TRANS("T-design (48)"), LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_48);
    CBoutputDirsPreset->addItem (TRANS("T-design (60)"), LOUDSPEAKER_ARRAY_PRESET_T_DESIGN_60);

    /* source coordinate viewport */
    outputCoordsVP.reset (new Viewport ("new viewport"));
    addAndMakeVisible (outputCoordsVP.get());
    outputCoordsView_handle = new outputCoordsView(p, MAX_NUM_CHANNELS, hosirrlib_getNumLoudspeakers(hHS));
    outputCoordsVP->setViewedComponent (outputCoordsView_handle);
    outputCoordsVP->setScrollBarsShown (true, false);
    outputCoordsVP->setAlwaysOnTop(true);
    outputCoordsVP->setBounds(455, 180, 184, 181);
    outputCoordsView_handle->setNCH(hosirrlib_getNumLoudspeakers(hHS));

    /* file loader */
    addAndMakeVisible (fileChooser);
    fileChooser.addListener (this);
    fileChooser.setBounds (22, 65, 196, 18);
    fileChooser.setColour(TextButton::buttonColourId, Colour (0xff187252));
    formatManager.registerBasicFormats();
    durationInSeconds = 0.0f;
    if(processor.getLoadWavDirectory() != TRANS("no_file")){
        fileChooser.setCurrentFile(processor.getLoadWavDirectory(), true);
        loadWavFile();
    }

    /* ProgressBar */
    RenderingInProgress = 0;
    progress = 0.0;
    progressbar.setBounds(getLocalBounds().getCentreX()-175, getLocalBounds().getCentreY()-17, 350, 35);
    progressbar.ProgressBar::setAlwaysOnTop(true);
    progressbar.setColour(ProgressBar::backgroundColourId, Colours::gold);
    progressbar.setColour(ProgressBar::foregroundColourId, Colours::white);

    /* RIR display */
    RIRviewVP.reset (new Viewport ("new viewport"));
    addAndMakeVisible (RIRviewVP.get());
    const int RIRview_width = 400;
    const int RIRview_height = 104;
    thumbnailComp = new RIRview(2048, formatManager, thumbnailCache, RIRview_width, RIRview_height);
    RIRviewVP->setViewedComponent(thumbnailComp);
    RIRviewVP->setBounds(22, 254, RIRview_width+8, RIRview_height);
    dispTimeTrim = 1.0f;
    dispGain_dB = 0.0f;
    thumbnailComp->setTimeTrim(dispTimeTrim);
    thumbnailComp->setGain_dB(dispGain_dB);
    currentView = RIR_VIEW_SH_LABELS;
    thumbnailComp->setLabelsView(currentView);
    CBdisplayRIR->addItem (TRANS("Ambi RIR"), RIR_VIEW_SH_LABELS);
    ///CBdisplayRIR->addItem (TRANS("LS RIR"), RIR_VIEW_LS_LABELS);

    /* grab current parameter settings */
    CBchFormat->setSelectedId(hosirrlib_getChOrder(hHS), sendNotification);
    CBnormScheme->setSelectedId(hosirrlib_getNormType(hHS), sendNotification);
    CBchFormat->setItemEnabled(CH_FUMA, hosirrlib_getAnalysisOrder(hHS)==ANALYSIS_ORDER_FIRST ? true : false);
    CBnormScheme->setItemEnabled(NORM_FUMA, hosirrlib_getAnalysisOrder(hHS)==ANALYSIS_ORDER_FIRST ? true : false);
    SL_displayGain->setValue(dispGain_dB, dontSendNotification);
    SL_displayTimeTrim->setValue(dispTimeTrim, dontSendNotification);
    CBdisplayRIR->setSelectedId(currentView, dontSendNotification);
    SL_windowSize->setValue(hosirrlib_getWindowLength(hHS), dontSendNotification);
    SL_wetDryBalance->setValue(hosirrlib_getWetDryBalance(hHS), dontSendNotification);
    tb_BroadBand1stPeak->setToggleState((bool)hosirrlib_getBroadBandFirstPeakFLAG(hHS), dontSendNotification);
    tb_saveRIR->setEnabled(hosirrlib_getLsRIRstatus(hHS)==LS_RIR_STATUS_RENDERED ? true : false);
    tb_render->setEnabled(hosirrlib_getAmbiRIRstatus(hHS)==AMBI_RIR_STATUS_LOADED ? true : false);

    /* tooltips */
    CBanaOrder->setTooltip("Analysis/Rendering order. Note that HOSIRR requires (order+1)^2 channels as input. This can be the same or lower than the order of the loaded Ambisonic RIR.");
    fileChooser.setTooltip("Load your Ambisonic RIR wav file here. Please note: make sure the Ambisonic format of the RIR matches the \"Format\" settings below.");
    SL_num_loudspeakers->setTooltip("Number of loudspeakers to render to.");
    CBchFormat->setTooltip("Ambisonic channel ordering convention (Note that AmbiX: ACN & SN3D).");
    CBnormScheme->setTooltip("Ambisonic normalisation scheme (Note that AmbiX: ACN & SN3D).");
    CBoutputDirsPreset->setTooltip("Presets for loudspeaker arrangements to render to.");
    tb_loadJSON->setTooltip("Loads loudspeaker directions from a JSON file. The JSON file format follows the same convention as the one employed by the IEM plugin suite (https://plugins.iem.at/docs/configurationfiles/).");
    tb_saveJSON->setTooltip("Saves the current loudspeaker directions to a JSON file. The JSON file format follows the same convention as the one employed by the IEM plugin suite (https://plugins.iem.at/docs/configurationfiles/).");
    tb_BroadBand1stPeak->setTooltip("This option isolates the first peak in the response and renders it based on a broad-band DoA estimate. This can help reduce colouration is certain cases.");
    SL_wetDryBalance->setTooltip("When set to 1.0 (default), the levels for the direct and diffuse streams are determined by the analysed diffuseness. However, this parameter may be used to manipulate this balance, either creatively or as a correction term. Note, 2.0: fully direct (dry), 0.0: fully diffuse (wet).");
    SL_windowSize->setTooltip("Windowing size, in samples, (note this implementation of HOSIRR employs 50% overlap).");
    CBdisplayRIR->setTooltip("Switch between displaying the input Ambisonic RIR, or the output Loudspeaker array RIR.");
    SL_displayTimeTrim->setTooltip("Can be used to shorten the RIR plotted.");
    SL_displayGain->setTooltip("Can be used to boost/attenuate the RIR plotted.");
    tb_render->setTooltip("Click to render the input Ambisonic RIR and create the output loudspeaker array RIR; using the HOSIRR algorithm, with the currently configured settings.");
    tb_saveRIR->setTooltip("Click to save the currently rendering loudspeaker array RIR to disk (.wav). Make sure to click \"Render\" first!");
    label_inputOrder->setTooltip("Spherical harmonic order of the currently loaded Ambisonic RIR.");
    label_inputSampleRate->setTooltip("SampleRate of the currently loaded Ambisonic RIR.");
    label_inputLength->setTooltip("Length (in seconds) of the currently loaded Ambisonic RIR.");

    /* Plugin description */
    pluginDescription.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (pluginDescription.get());
    pluginDescription->setBounds (0, 0, 460, 32);
    pluginDescription->setAlpha(0.0f);
    pluginDescription->setEnabled(false);
    pluginDescription->setTooltip(TRANS("An implementation of the Higher-order Spatial Impulse Response Rendering (HO-SIRR) method, which can synthesise output loudspeaker array room impulse responses (RIRs) using input spherical harmonic (Ambisonic/B-Format) RIRs of arbitrary order. The method makes assumptions regarding the composition of the sound-field and extracts spatial parameters over time and frequency, which allows it to map the input to the output in an adaptive and more informed manner when compared to linear methods such as Ambisonic decoding. The idea is that you then convolve a monophonic source with this loudspeaker array RIR, and it will be reproduced and exhibit all of the spatial characteristics of the captured space over your loudspeaker set-up.\n\n")+
                                  TRANS("The suggested workflow is: \n   1) Measure a room impulse response (RIR) of a space with a spherical microphone array (e.g. using HAART), and convert it into an Ambisonic/B-format RIR (e.g. using sparta_array2sh). \n   2) Load this B-Format/Ambisonic RIR into the HOSIRR App/plug-in and specify your loudspeaker array layout and desired rendering configuration (although, the default should suffice for most purposes).\n   3) Click \"Render\", and then \"Save\", to export the resulting loudspeaker array RIR as a multi-channel .wav file.\n   4) Then convolve this loudspeaker array RIR with a monophonic source signal, and it will be reproduced over the loudspeaker array (also exhibiting the spatial characteristics of the captured space). Plug-ins such as Xvolver, sparta_matrixconv, and mcfx_convolver, are well suited to this convolution task.\n\n "));
    addAndMakeVisible (publicationLink);
    publicationLink.setColour (HyperlinkButton::textColourId, Colours::lightblue);
    publicationLink.setBounds(getBounds().getWidth()-80, 4, 80, 12);
    publicationLink.setJustificationType(Justification::centredLeft);

    /* Specify screen refresh rate */
    startTimer(80);//80); /*ms (40ms = 25 frames per second) */

    /* warnings */
    currentWarning = k_warning_none;
}

PluginEditor::~PluginEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void PluginEditor::paint (juce::Graphics& g)
{
    using namespace ColoursUI;

    drawPluginBackgroundAndBanner(g, getBounds());

    /* Panels */
    drawPanel(g, {12, 58,213,32}, panelFill,      panelStroke);
    drawPanel(g, {12, 58,213,32}, panelFillLight, panelStroke);
    drawPanel(g, {12, 88,213,32}, panelFill,      panelStroke);
    drawPanel(g, {12,119,213,81}, panelFill,      panelStroke);
    drawPanel(g, {230, 58,210,31}, panelFill,      panelStroke);
    drawPanel(g, {230, 58,210,31}, panelFillLight, panelStroke);
    drawPanel(g, {230, 89,210,111}, panelFill, panelStroke);
    drawPanel(g, {446, 58,196,32}, panelFill,      panelStroke);
    drawPanel(g, {446, 58,196,32}, panelFillLight, panelStroke);
    drawPanel(g, {446, 90,196,64}, panelFill,      panelStroke);
    drawPanel(g, {446,153,196,215}, panelFill,     panelStroke);
    drawPanel(g, {12,224,428,144}, panelFill, panelStroke);

    /* Title */
    drawLabel(g, {16,1,480,32}, "Higher-Order Spatial Impulse Response Renderer", 18.8f);

    /* Section titles */
    drawLabel(g, {48,34,144,30}, "Input Ambi RIR",   15.f, juce::Justification::centred);
    drawLabel(g, {472,33,144,30}, "Output Lspkr RIR", 15.f, juce::Justification::centred);
    drawLabel(g, {300,33,93,30},  "Renderer",         15.f);

    /* Left column labels */
    drawLabel(g, {20, 89,132,30}, "Format:",          14.5f);
    drawLabel(g, {20,121,132,30}, "RIR order:",       14.5f);
    drawLabel(g, {20,145,132,30}, "RIR length (s):",  14.5f);
    drawLabel(g, {20,169,132,30}, "RIR Samplerate:",  14.5f);

    /* Middle column labels */
    drawLabel(g, {236, 88,132,30}, "Analysis Order:",     14.5f);
    drawLabel(g, {236,116,132,30}, "Window Length:",      14.5f);
    drawLabel(g, {236,144,193,31}, "Wet/Dry:",            14.5f);
    drawLabel(g, {236,171,188,33}, "Broad-Band First Peak:", 14.5f);

    /* Right column labels */
    drawLabel(g, {455, 90,67,30},  "Presets:",        15.f);
    drawLabel(g, {455,118,169,30}, "Num Loudspeakers:",  15.f);

    /* Azimuth/Elevation header */
    drawLabel(g, {501,154,105,28},
              juce::CharPointer_UTF8("Azi\xc2\xb0   #   Elev\xc2\xb0"),
              15.f);

    /* RIR Display block */
    drawLabel(g, {176,199,108,30}, "RIR Display", 15.f, juce::Justification::centred);
    drawLabel(g, {132,223,132,30}, "Time Trim:",  14.5f);
    drawLabel(g, {305,223,132,30}, "Scale:",      14.5f);

    /* disp version number */
    g.setColour(Colours::white);
    g.setFont(juce::FontOptions (11.00f, Font::plain));
    g.drawText(TRANS("Ver ") + JucePlugin_VersionString + BUILD_VER_SUFFIX + TRANS(", Build Date ") + __DATE__ + TRANS(" "),
        437, 16, 530, 11,
        Justification::centredLeft, true);

    /* draw rect around RIR display window */
    g.setColour (Colour (0xffb9b9b9));
    g.drawRect(RIRviewVP->getBounds().expanded(1, 1),1.0f);

    /* display warning message */
    g.setColour(Colours::red);
    g.setFont(juce::FontOptions (11.00f, Font::plain));
    switch (currentWarning){
        case k_warning_none:
            break;
    }
}

void PluginEditor::resized()
{
}

void PluginEditor::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == CBoutputDirsPreset.get())
    {
        hosirrlib_setOutputConfigPreset(hHS, CBoutputDirsPreset->getSelectedId());
        
        processor.setParameterValue("numLoudspeakers", hosirrlib_getNumLoudspeakers(hHS));
        for(int i=0; i<hosirrlib_getNumLoudspeakers(hHS); i++){
            processor.setParameterValue("azim" + juce::String(i), hosirrlib_getLoudspeakerAzi_deg(hHS,i));
            processor.setParameterValue("elev" + juce::String(i), hosirrlib_getLoudspeakerElev_deg(hHS,i));
        }
    }
    else if (comboBoxThatHasChanged == CBdisplayRIR.get())
    {
        thumbnailComp->setLabelsView((RIR_VIEW_WAV_LABELS)CBdisplayRIR->getSelectedId());
        switch(CBdisplayRIR->getSelectedId()){
            case RIR_VIEW_LS_LABELS:
                //thumbnailComp->setFile(<#const File &file#>)
                break;
            case RIR_VIEW_SH_LABELS:
                thumbnailComp->setFile(fileChooser.getCurrentFile());
                break;
        }
    }
}

void PluginEditor::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == SL_wetDryBalance.get())
    {
        hosirrlib_setWetDryBalance(hHS, (float)SL_wetDryBalance->getValue());
    }
    else if (sliderThatWasMoved == SL_windowSize.get())
    {
        hosirrlib_setWindowLength(hHS, (int)SL_windowSize->getValue());
    }
    else if (sliderThatWasMoved == SL_displayGain.get())
    {
        dispGain_dB = (float)SL_displayGain->getValue();
        thumbnailComp->setGain_dB(dispGain_dB);
    }
    else if (sliderThatWasMoved == SL_displayTimeTrim.get())
    {
        dispTimeTrim = (float)SL_displayTimeTrim->getValue();
        thumbnailComp->setTimeTrim(dispTimeTrim);
    }
}

void PluginEditor::buttonClicked (juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == tb_loadJSON.get())
    {
        chooser = std::make_unique<juce::FileChooser> ("Load configuration...",
                                                       processor.getLastJSONDir().exists() ? processor.getLastJSONDir() : File::getSpecialLocation (File::userHomeDirectory),
                                                       "*.json");
        auto chooserFlags = juce::FileBrowserComponent::openMode
                                  | juce::FileBrowserComponent::canSelectFiles;
        chooser->launchAsync (chooserFlags, [this] (const FileChooser& fc) {
            auto file = fc.getResult();
            if (file != File{}){
                processor.setLastJSONDir(file.getParentDirectory());
                processor.loadConfiguration (file);
            }
        });
    }
    else if (buttonThatWasClicked == tb_saveJSON.get())
    {
        chooser = std::make_unique<juce::FileChooser> ("Save configuration...",
                                                       processor.getLastJSONDir().exists() ? processor.getLastJSONDir() : File::getSpecialLocation (File::userHomeDirectory),
                                                       "*.json");
        auto chooserFlags = juce::FileBrowserComponent::saveMode;
        chooser->launchAsync (chooserFlags, [this] (const FileChooser& fc) {
            auto file = fc.getResult();
            if (file != File{}) {
                processor.setLastJSONDir(file.getParentDirectory());
                processor.saveConfigurationToFile (file);
            }
        });
    }
    else if (buttonThatWasClicked == tb_render.get())
    {
        if(!RenderingInProgress){
            try{
                std::thread render_thread(hosirrlib_render, hHS);
                render_thread.detach();
            } catch (const std::exception& exception) {
                std::cout << "Could not create thread to render" << exception.what() << std::endl;
            }
        }
    }
    else if (buttonThatWasClicked == tb_saveRIR.get())
    {
        if(hosirrlib_getLsRIRstatus(hHS)==LS_RIR_STATUS_RENDERED){
            /* ask user, where to save */
            chooser = std::make_unique<juce::FileChooser> ("Save wav...",
                                                           processor.getSaveWavDirectory().exists() ? processor.getSaveWavDirectory() : File::getSpecialLocation (File::userHomeDirectory),
                                                           "*.wav");
            auto chooserFlags = juce::FileBrowserComponent::saveMode;
            chooser->launchAsync (chooserFlags, [this] (const FileChooser& fc) {
                auto file = fc.getResult();
                if (file != File{}) {
                    processor.setSaveWavDirectory(file.getParentDirectory());

                    /* fill audio buffer */
                    AudioBuffer<float> buffer;
                    buffer.setSize(hosirrlib_getNumLoudspeakers(hHS), hosirrlib_getAmbiRIRlength_samples(hHS));
                    buffer.clear();
                    float* const* lsRIR = buffer.getArrayOfWritePointers();
                    hosirrlib_getLsRIR(hHS, lsRIR);

                    /* write audio buffer to disk */
                    WavAudioFormat wavFormat;
                    std::unique_ptr<AudioFormatWriter> writer;
                    file.deleteFile();
                    writer.reset (wavFormat.createWriterFor (new FileOutputStream (file), (double)hosirrlib_getAmbiRIRsampleRate(hHS),
                                                             (unsigned int)hosirrlib_getNumLoudspeakers(hHS), 24, {}, 0));
                    if (writer != nullptr){
                        writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
                    }
                    writer = nullptr;
                }
            });
        }
    }
    else if (buttonThatWasClicked == tb_BroadBand1stPeak.get())
    {
        hosirrlib_setBroadBandFirstPeakFLAG(hHS, tb_BroadBand1stPeak->getToggleState());
    }
}

void PluginEditor::timerCallback()
{
    /* parameters whos values can change internally should be periodically refreshed */
    outputCoordsView_handle->setNCH(hosirrlib_getNumLoudspeakers(hHS));
    CBchFormat->setSelectedId(hosirrlib_getChOrder(hHS), sendNotification);
    CBnormScheme->setSelectedId(hosirrlib_getNormType(hHS), sendNotification);
    CBchFormat->setItemEnabled(CH_FUMA, hosirrlib_getAnalysisOrder(hHS)==ANALYSIS_ORDER_FIRST ? true : false);
    CBnormScheme->setItemEnabled(NORM_FUMA, hosirrlib_getAnalysisOrder(hHS)==ANALYSIS_ORDER_FIRST ? true : false);
    label_inputOrder->setText(String(hosirrlib_getAmbiRIRinputOrder(hHS)), dontSendNotification);
    label_inputLength->setText(String(hosirrlib_getAmbiRIRlength_seconds(hHS)), dontSendNotification);
    label_inputSampleRate->setText(String(hosirrlib_getAmbiRIRsampleRate(hHS)), dontSendNotification);
    SL_windowSize->setValue(hosirrlib_getWindowLength(hHS), dontSendNotification);
    for(int i=1; i<=HOSIRR_MAX_SH_ORDER; i++)
        CBanaOrder->setItemEnabled(i, i<=hosirrlib_getAmbiRIRinputOrder(hHS));

    /* enable/disable */
    tb_saveRIR->setEnabled(hosirrlib_getLsRIRstatus(hHS)==LS_RIR_STATUS_RENDERED ? true : false);
    tb_render->setEnabled(hosirrlib_getAmbiRIRstatus(hHS)==AMBI_RIR_STATUS_LOADED &&
                          hosirrlib_getLsRIRstatus(hHS)!=LS_RIR_STATUS_RENDERED &&
                          hosirrlib_getLsRIRstatus(hHS)!=LS_RIR_STATUS_RENDEREDING_ONGOING? true : false);

    if(hosirrlib_getLsRIRstatus(hHS)==LS_RIR_STATUS_RENDEREDING_ONGOING){
        CBoutputDirsPreset->setEnabled(false);
        SL_num_loudspeakers->setEnabled(false);
        CBchFormat->setEnabled(false);
        CBnormScheme->setEnabled(false);
        tb_loadJSON->setEnabled(false);
        tb_saveJSON->setEnabled(false);
        CBanaOrder->setEnabled(false);
        SL_wetDryBalance->setEnabled(false);
        SL_windowSize->setEnabled(false);
        tb_BroadBand1stPeak->setEnabled(false);
        fileChooser.setEnabled(false);
        outputCoordsVP->setEnabled(false);
    }
    else{
        CBoutputDirsPreset->setEnabled(true);
        SL_num_loudspeakers->setEnabled(true);
        CBchFormat->setEnabled(true);
        CBnormScheme->setEnabled(true);
        tb_loadJSON->setEnabled(true);
        tb_saveJSON->setEnabled(true);
        CBanaOrder->setEnabled(true);
        SL_wetDryBalance->setEnabled(true);
        SL_windowSize->setEnabled(true);
        tb_BroadBand1stPeak->setEnabled(true);
        fileChooser.setEnabled(true);
        outputCoordsVP->setEnabled(true);
    }

    /* display warning message, if needed */
    currentWarning = k_warning_none;
    if(hosirrlib_getLsRIRstatus(hHS)==LS_RIR_STATUS_RENDEREDING_ONGOING){
        addAndMakeVisible(progressbar);
        progress = (double)hosirrlib_getProgress0_1(hHS);
        char text[HOSIRR_PROGRESSTEXT_CHAR_LENGTH];
        hosirrlib_getProgressText(hHS, (char*)text);
        progressbar.setTextToDisplay(String(text));
    }
    else
        removeChildComponent(&progressbar);
}
