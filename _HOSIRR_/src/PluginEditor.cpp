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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

//[/MiscUserDefs]

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor* ownerFilter)
    : AudioProcessorEditor(ownerFilter), progressbar(progress), fileChooser ("File", File(), true, false, false, "*.wav", String(),
      "Load *.wav File"), thumbnailCache (5)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    CBoutputDirsPreset.reset (new ComboBox ("new combo box"));
    addAndMakeVisible (CBoutputDirsPreset.get());
    CBoutputDirsPreset->setEditableText (false);
    CBoutputDirsPreset->setJustificationType (Justification::centredLeft);
    CBoutputDirsPreset->setTextWhenNothingSelected (TRANS("Default"));
    CBoutputDirsPreset->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    CBoutputDirsPreset->addItem (TRANS("Default"), 1);
    CBoutputDirsPreset->addListener (this);

    CBoutputDirsPreset->setBounds (520, 96, 112, 20);

    SL_num_loudspeakers.reset (new Slider ("new slider"));
    addAndMakeVisible (SL_num_loudspeakers.get());
    SL_num_loudspeakers->setRange (1, 64, 1);
    SL_num_loudspeakers->setSliderStyle (Slider::LinearHorizontal);
    SL_num_loudspeakers->setTextBoxStyle (Slider::TextBoxRight, false, 60, 20);
    SL_num_loudspeakers->addListener (this);

    SL_num_loudspeakers->setBounds (592, 124, 40, 20);

    CBchFormat.reset (new ComboBox ("new combo box"));
    addAndMakeVisible (CBchFormat.get());
    CBchFormat->setEditableText (false);
    CBchFormat->setJustificationType (Justification::centredLeft);
    CBchFormat->setTextWhenNothingSelected (TRANS("ACN"));
    CBchFormat->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    CBchFormat->addListener (this);

    CBchFormat->setBounds (77, 95, 68, 20);

    CBnormScheme.reset (new ComboBox ("new combo box"));
    addAndMakeVisible (CBnormScheme.get());
    CBnormScheme->setEditableText (false);
    CBnormScheme->setJustificationType (Justification::centredLeft);
    CBnormScheme->setTextWhenNothingSelected (TRANS("N3D"));
    CBnormScheme->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    CBnormScheme->addListener (this);

    CBnormScheme->setBounds (147, 95, 72, 20);

    tb_loadJSON.reset (new TextButton ("new button"));
    addAndMakeVisible (tb_loadJSON.get());
    tb_loadJSON->setButtonText (TRANS("Import"));
    tb_loadJSON->setConnectedEdges (Button::ConnectedOnRight);
    tb_loadJSON->addListener (this);
    tb_loadJSON->setColour (TextButton::buttonColourId, Colour (0xff14889e));

    tb_loadJSON->setBounds (456, 161, 34, 14);

    tb_saveJSON.reset (new TextButton ("new button"));
    addAndMakeVisible (tb_saveJSON.get());
    tb_saveJSON->setButtonText (TRANS("Export"));
    tb_saveJSON->setConnectedEdges (Button::ConnectedOnLeft);
    tb_saveJSON->addListener (this);
    tb_saveJSON->setColour (TextButton::buttonColourId, Colour (0xff224d97));
    tb_saveJSON->setColour (TextButton::buttonOnColourId, Colour (0xff181f9a));

    tb_saveJSON->setBounds (600, 161, 34, 14);

    CBanaOrder.reset (new ComboBox ("new combo box"));
    addAndMakeVisible (CBanaOrder.get());
    CBanaOrder->setEditableText (false);
    CBanaOrder->setJustificationType (Justification::centredLeft);
    CBanaOrder->setTextWhenNothingSelected (TRANS("Default"));
    CBanaOrder->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    CBanaOrder->addListener (this);

    CBanaOrder->setBounds (344, 95, 88, 20);

    tb_render.reset (new TextButton ("new button"));
    addAndMakeVisible (tb_render.get());
    tb_render->setButtonText (TRANS("Render"));
    tb_render->addListener (this);
    tb_render->setColour (TextButton::buttonColourId, Colour (0xff9e8c14));

    tb_render->setBounds (264, 65, 144, 18);

    tb_saveRIR.reset (new TextButton ("new button"));
    addAndMakeVisible (tb_saveRIR.get());
    tb_saveRIR->setButtonText (TRANS("Save"));
    tb_saveRIR->addListener (this);
    tb_saveRIR->setColour (TextButton::buttonColourId, Colour (0xff9e2f14));

    tb_saveRIR->setBounds (480, 65, 136, 18);

    label_inputOrder.reset (new Label ("new label",
                                       String()));
    addAndMakeVisible (label_inputOrder.get());
    label_inputOrder->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label_inputOrder->setJustificationType (Justification::centredLeft);
    label_inputOrder->setEditable (false, false, false);
    label_inputOrder->setColour (Label::outlineColourId, Colour (0x68a3a2a2));
    label_inputOrder->setColour (TextEditor::textColourId, Colours::black);
    label_inputOrder->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label_inputOrder->setBounds (130, 128, 88, 20);

    label_inputLength.reset (new Label ("new label",
                                        String()));
    addAndMakeVisible (label_inputLength.get());
    label_inputLength->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label_inputLength->setJustificationType (Justification::centredLeft);
    label_inputLength->setEditable (false, false, false);
    label_inputLength->setColour (Label::outlineColourId, Colour (0x68a3a2a2));
    label_inputLength->setColour (TextEditor::textColourId, Colours::black);
    label_inputLength->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label_inputLength->setBounds (130, 152, 88, 20);

    label_inputSampleRate.reset (new Label ("new label",
                                            String()));
    addAndMakeVisible (label_inputSampleRate.get());
    label_inputSampleRate->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label_inputSampleRate->setJustificationType (Justification::centredLeft);
    label_inputSampleRate->setEditable (false, false, false);
    label_inputSampleRate->setColour (Label::outlineColourId, Colour (0x68a3a2a2));
    label_inputSampleRate->setColour (TextEditor::textColourId, Colours::black);
    label_inputSampleRate->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label_inputSampleRate->setBounds (130, 176, 88, 20);

    SL_wetDryBalance.reset (new Slider ("new slider"));
    addAndMakeVisible (SL_wetDryBalance.get());
    SL_wetDryBalance->setRange (0, 2, 0.01);
    SL_wetDryBalance->setSliderStyle (Slider::LinearHorizontal);
    SL_wetDryBalance->setTextBoxStyle (Slider::TextBoxRight, false, 50, 20);
    SL_wetDryBalance->setColour (Slider::backgroundColourId, Colour (0xff5c5d5e));
    SL_wetDryBalance->setColour (Slider::trackColourId, Colour (0xff315b6d));
    SL_wetDryBalance->addListener (this);

    SL_wetDryBalance->setBounds (312, 151, 120, 20);

    SL_windowSize.reset (new Slider ("new slider"));
    addAndMakeVisible (SL_windowSize.get());
    SL_windowSize->setRange (32, 256, 2);
    SL_windowSize->setSliderStyle (Slider::LinearHorizontal);
    SL_windowSize->setTextBoxStyle (Slider::TextBoxRight, false, 60, 20);
    SL_windowSize->addListener (this);

    SL_windowSize->setBounds (360, 122, 71, 20);

    CBdisplayRIR.reset (new ComboBox ("new combo box"));
    addAndMakeVisible (CBdisplayRIR.get());
    CBdisplayRIR->setEditableText (false);
    CBdisplayRIR->setJustificationType (Justification::centredLeft);
    CBdisplayRIR->setTextWhenNothingSelected (TRANS("Ambi RIR"));
    CBdisplayRIR->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    CBdisplayRIR->addListener (this);

    CBdisplayRIR->setBounds (21, 229, 99, 18);

    SL_displayGain.reset (new Slider ("new slider"));
    addAndMakeVisible (SL_displayGain.get());
    SL_displayGain->setRange (-24, 24, 0.01);
    SL_displayGain->setSliderStyle (Slider::LinearHorizontal);
    SL_displayGain->setTextBoxStyle (Slider::NoTextBox, false, 50, 20);
    SL_displayGain->setColour (Slider::backgroundColourId, Colour (0xff5c5d5e));
    SL_displayGain->setColour (Slider::trackColourId, Colour (0xff315b6d));
    SL_displayGain->addListener (this);

    SL_displayGain->setBounds (344, 229, 88, 20);

    SL_displayTimeTrim.reset (new Slider ("new slider"));
    addAndMakeVisible (SL_displayTimeTrim.get());
    SL_displayTimeTrim->setRange (0.01, 1, 0.01);
    SL_displayTimeTrim->setSliderStyle (Slider::LinearHorizontal);
    SL_displayTimeTrim->setTextBoxStyle (Slider::NoTextBox, false, 50, 20);
    SL_displayTimeTrim->setColour (Slider::backgroundColourId, Colour (0xff5c5d5e));
    SL_displayTimeTrim->setColour (Slider::trackColourId, Colour (0xff315b6d));
    SL_displayTimeTrim->addListener (this);

    SL_displayTimeTrim->setBounds (208, 229, 88, 20);

    tb_BroadBand1stPeak.reset (new ToggleButton ("new toggle button"));
    addAndMakeVisible (tb_BroadBand1stPeak.get());
    tb_BroadBand1stPeak->setButtonText (String());
    tb_BroadBand1stPeak->addListener (this);

    tb_BroadBand1stPeak->setBounds (410, 174, 26, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (656, 380);


    //[Constructor] You can add your own custom stuff here..

    /* handle to pluginProcessor */
	hVst = ownerFilter;
    hHS = hVst->getFXHandle();

    /* init OpenGL */
    openGLContext.setMultisamplingEnabled(true);
    openGLContext.attachTo(*this);

    /* remove slider bit of these sliders */
    SL_num_loudspeakers->setColour(Slider::trackColourId, Colours::transparentBlack);
    SL_num_loudspeakers->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    SL_num_loudspeakers->setSliderSnapsToMousePosition(false);
    SL_windowSize->setColour(Slider::trackColourId, Colours::transparentBlack);
    SL_windowSize->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    SL_windowSize->setSliderSnapsToMousePosition(false);

    /* add analysis order options */
    CBanaOrder->addItem (TRANS("1st order"), ANALYSIS_ORDER_FIRST);
    CBanaOrder->addItem (TRANS("2nd order"), ANALYSIS_ORDER_SECOND);
    CBanaOrder->addItem (TRANS("3rd order"), ANALYSIS_ORDER_THIRD);
    CBanaOrder->addItem (TRANS("4th order"), ANALYSIS_ORDER_FOURTH);
    CBanaOrder->addItem (TRANS("5th order"), ANALYSIS_ORDER_FIFTH);
    CBanaOrder->addItem (TRANS("6th order"), ANALYSIS_ORDER_SIXTH);
    CBanaOrder->addItem (TRANS("7th order"), ANALYSIS_ORDER_SEVENTH);

    /* ambi convention options */
    CBchFormat->addItem (TRANS("ACN"), CH_ACN);
    CBchFormat->addItem (TRANS("FuMa"), CH_FUMA);
    CBnormScheme->addItem (TRANS("N3D"), NORM_N3D);
    CBnormScheme->addItem (TRANS("SN3D"), NORM_SN3D);
    CBnormScheme->addItem (TRANS("FuMa"), NORM_FUMA);

    /* add loudspeaker preset options */
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
    outputCoordsView_handle = new outputCoordsView(ownerFilter, MAX_NUM_CHANNELS, hosirrlib_getNumLoudspeakers(hHS));
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
    if(hVst->getLoadWavDirectory() != TRANS("no_file")){
        fileChooser.setCurrentFile(hVst->getLoadWavDirectory(), true);
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
    CBdisplayRIR->addItem (TRANS("LS RIR"), RIR_VIEW_LS_LABELS);

    /* grab current parameter settings */
    CBanaOrder->setSelectedId(hosirrlib_getAnalysisOrder(hHS), dontSendNotification);
    SL_num_loudspeakers->setValue(hosirrlib_getNumLoudspeakers(hHS),dontSendNotification);
    CBchFormat->setSelectedId(hosirrlib_getChOrder(hHS), dontSendNotification);
    CBnormScheme->setSelectedId(hosirrlib_getNormType(hHS), dontSendNotification);
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

	/* Specify screen refresh rate */
    startTimer(80);//80); /*ms (40ms = 25 frames per second) */

    /* warnings */
    currentWarning = k_warning_none;

    //[/Constructor]
}

PluginEditor::~PluginEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    CBoutputDirsPreset = nullptr;
    SL_num_loudspeakers = nullptr;
    CBchFormat = nullptr;
    CBnormScheme = nullptr;
    tb_loadJSON = nullptr;
    tb_saveJSON = nullptr;
    CBanaOrder = nullptr;
    tb_render = nullptr;
    tb_saveRIR = nullptr;
    label_inputOrder = nullptr;
    label_inputLength = nullptr;
    label_inputSampleRate = nullptr;
    SL_wetDryBalance = nullptr;
    SL_windowSize = nullptr;
    CBdisplayRIR = nullptr;
    SL_displayGain = nullptr;
    SL_displayTimeTrim = nullptr;
    tb_BroadBand1stPeak = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    outputCoordsVP = nullptr;
    RIRviewVP = nullptr;
    //[/Destructor]
}

//==============================================================================
void PluginEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    {
        int x = 0, y = 192, width = 656, height = 188;
        Colour fillColour1 = Colour (0xff1c3949), fillColour2 = Colour (0xff071e22);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       8.0f - 0.0f + x,
                                       384.0f - 192.0f + y,
                                       fillColour2,
                                       8.0f - 0.0f + x,
                                       312.0f - 192.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 0, y = 30, width = 656, height = 163;
        Colour fillColour1 = Colour (0xff1c3949), fillColour2 = Colour (0xff071e22);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       8.0f - 0.0f + x,
                                       32.0f - 30.0f + y,
                                       fillColour2,
                                       8.0f - 0.0f + x,
                                       88.0f - 30.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 12, y = 119, width = 213, height = 81;
        Colour fillColour = Colour (0x10f4f4f4);
        Colour strokeColour = Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        float x = 1.0f, y = 1.0f, width = 654.0f, height = 32.0f;
        Colour fillColour1 = Colour (0xff061c20), fillColour2 = Colour (0xff1c3949);
        Colour strokeColour = Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       0.0f - 1.0f + x,
                                       32.0f - 1.0f + y,
                                       fillColour2,
                                       656.0f - 1.0f + x,
                                       24.0f - 1.0f + y,
                                       false));
        g.fillRoundedRectangle (x, y, width, height, 5.000f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 5.000f, 2.000f);
    }

    {
        int x = 0, y = 0, width = 656, height = 2;
        Colour strokeColour = Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 2);

    }

    {
        int x = 12, y = 88, width = 213, height = 32;
        Colour fillColour = Colour (0x10f4f4f4);
        Colour strokeColour = Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 12, y = 58, width = 213, height = 32;
        Colour fillColour = Colour (0x10f4f4f4);
        Colour strokeColour = Colour (0x35a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 446, y = 153, width = 196, height = 215;
        Colour fillColour = Colour (0x10f4f4f4);
        Colour strokeColour = Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 230, y = 89, width = 210, height = 111;
        Colour fillColour = Colour (0x10f4f4f4);
        Colour strokeColour = Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 446, y = 90, width = 196, height = 64;
        Colour fillColour = Colour (0x10f4f4f4);
        Colour strokeColour = Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 455, y = 90, width = 67, height = 30;
        String text (TRANS("Presets: "));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 499, y = 154, width = 105, height = 28;
        String text (CharPointer_UTF8 ("Azi\xc2\xb0   #   Elev\xc2\xb0"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 455, y = 118, width = 169, height = 30;
        String text (TRANS("N Loudspeakers"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 48, y = 34, width = 144, height = 30;
        String text (TRANS("Input Ambi RIR"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = 472, y = 33, width = 144, height = 30;
        String text (TRANS("Output Lspkr RIR"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = 20, y = 89, width = 132, height = 30;
        String text (TRANS("Format:"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 12, y = 224, width = 428, height = 144;
        Colour fillColour = Colour (0x10f4f4f4);
        Colour strokeColour = Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 16, y = 1, width = 480, height = 32;
        String text (TRANS("Higher-Order Spatial Impulse Response Renderer"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (18.80f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 0, y = 0, width = 2, height = 380;
        Colour strokeColour = Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 2);

    }

    {
        int x = 654, y = 0, width = 2, height = 380;
        Colour strokeColour = Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 2);

    }

    {
        int x = 0, y = 378, width = 656, height = 2;
        Colour strokeColour = Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 2);

    }

    {
        int x = 236, y = 88, width = 132, height = 30;
        String text (TRANS("Analysis Order:"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 236, y = 116, width = 132, height = 30;
        String text (TRANS("Window Length: "));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 236, y = 144, width = 193, height = 31;
        String text (TRANS("Wet/Dry:"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 236, y = 171, width = 188, height = 33;
        String text (TRANS("Broad-Band First Peak: "));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 20, y = 121, width = 132, height = 30;
        String text (TRANS("RIR order: "));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 20, y = 145, width = 132, height = 30;
        String text (TRANS("RIR length (s): "));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 20, y = 169, width = 132, height = 30;
        String text (TRANS("RIR Samplerate: "));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 300, y = 33, width = 93, height = 30;
        String text (TRANS("Renderer"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 176, y = 199, width = 108, height = 30;
        String text (TRANS("RIR Display"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = 132, y = 223, width = 132, height = 30;
        String text (TRANS("Time Trim:"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 305, y = 223, width = 132, height = 30;
        String text (TRANS("Scale:"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (14.50f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centredLeft, true);
    }

    {
        int x = 12, y = 58, width = 213, height = 31;
        Colour fillColour = Colour (0x08f4f4f4);
        Colour strokeColour = Colour (0x35a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 230, y = 58, width = 210, height = 31;
        Colour fillColour = Colour (0x10f4f4f4);
        Colour strokeColour = Colour (0x35a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 230, y = 58, width = 210, height = 31;
        Colour fillColour = Colour (0x08f4f4f4);
        Colour strokeColour = Colour (0x35a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 446, y = 58, width = 196, height = 32;
        Colour fillColour = Colour (0x10f4f4f4);
        Colour strokeColour = Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 446, y = 58, width = 196, height = 32;
        Colour fillColour = Colour (0x08f4f4f4);
        Colour strokeColour = Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    //[UserPaint] Add your own custom painting code here..

    /* disp version number */
	g.setColour(Colours::white);
	g.setFont(Font(11.00f, Font::plain));
	g.drawText(TRANS("Ver ") + JucePlugin_VersionString + BUILD_VER_SUFFIX + TRANS(", Build Date ") + __DATE__ + TRANS(" "),
		437, 16, 530, 11,
		Justification::centredLeft, true);

    /* draw rect around RIR display window */
    g.setColour (Colour (0xffb9b9b9));
    g.drawRect(RIRviewVP->getBounds().expanded(1, 1),1.0f);

    /* display warning message */
    g.setColour(Colours::red);
    g.setFont(Font(11.00f, Font::plain));
    switch (currentWarning){
        case k_warning_none:
            break;
    }

    //[/UserPaint]
}

void PluginEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..

    //[/UserResized]
}

void PluginEditor::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == CBoutputDirsPreset.get())
    {
        //[UserComboBoxCode_CBoutputDirsPreset] -- add your combo box handling code here..
        hosirrlib_setOutputConfigPreset(hHS, CBoutputDirsPreset->getSelectedId());
        //[/UserComboBoxCode_CBoutputDirsPreset]
    }
    else if (comboBoxThatHasChanged == CBchFormat.get())
    {
        //[UserComboBoxCode_CBchFormat] -- add your combo box handling code here..
        hosirrlib_setChOrder(hHS, CBchFormat->getSelectedId());
        //[/UserComboBoxCode_CBchFormat]
    }
    else if (comboBoxThatHasChanged == CBnormScheme.get())
    {
        //[UserComboBoxCode_CBnormScheme] -- add your combo box handling code here..
        hosirrlib_setNormType(hHS, CBnormScheme->getSelectedId());
        //[/UserComboBoxCode_CBnormScheme]
    }
    else if (comboBoxThatHasChanged == CBanaOrder.get())
    {
        //[UserComboBoxCode_CBanaOrder] -- add your combo box handling code here..
        hosirrlib_setAnalysisOrder(hHS, CBanaOrder->getSelectedId());
        //[/UserComboBoxCode_CBanaOrder]
    }
    else if (comboBoxThatHasChanged == CBdisplayRIR.get())
    {
        //[UserComboBoxCode_CBdisplayRIR] -- add your combo box handling code here..
        thumbnailComp->setLabelsView((RIR_VIEW_WAV_LABELS)CBdisplayRIR->getSelectedId());
        switch(CBdisplayRIR->getSelectedId()){
            case RIR_VIEW_LS_LABELS:
                //thumbnailComp->setFile(<#const File &file#>)
                break;
            case RIR_VIEW_SH_LABELS:
                thumbnailComp->setFile(fileChooser.getCurrentFile());
                break;
        }
        //[/UserComboBoxCode_CBdisplayRIR]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void PluginEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == SL_num_loudspeakers.get())
    {
        //[UserSliderCode_SL_num_loudspeakers] -- add your slider handling code here..
        hosirrlib_setNumLoudspeakers(hHS, (int)SL_num_loudspeakers->getValue());
        //[/UserSliderCode_SL_num_loudspeakers]
    }
    else if (sliderThatWasMoved == SL_wetDryBalance.get())
    {
        //[UserSliderCode_SL_wetDryBalance] -- add your slider handling code here..
        hosirrlib_setWetDryBalance(hHS, (float)SL_wetDryBalance->getValue());
        //[/UserSliderCode_SL_wetDryBalance]
    }
    else if (sliderThatWasMoved == SL_windowSize.get())
    {
        //[UserSliderCode_SL_windowSize] -- add your slider handling code here..
        hosirrlib_setWindowLength(hHS, (int)SL_windowSize->getValue());
        //[/UserSliderCode_SL_windowSize]
    }
    else if (sliderThatWasMoved == SL_displayGain.get())
    {
        //[UserSliderCode_SL_displayGain] -- add your slider handling code here..
        dispGain_dB = (float)SL_displayGain->getValue();
        thumbnailComp->setGain_dB(dispGain_dB);
        //[/UserSliderCode_SL_displayGain]
    }
    else if (sliderThatWasMoved == SL_displayTimeTrim.get())
    {
        //[UserSliderCode_SL_displayTimeTrim] -- add your slider handling code here..
        dispTimeTrim = (float)SL_displayTimeTrim->getValue();
        thumbnailComp->setTimeTrim(dispTimeTrim);
        //[/UserSliderCode_SL_displayTimeTrim]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void PluginEditor::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == tb_loadJSON.get())
    {
        //[UserButtonCode_tb_loadJSON] -- add your button handler code here..
        FileChooser myChooser ("Load configuration...",
                               hVst->getLastJSONDir().exists() ? hVst->getLastJSONDir() : File::getSpecialLocation (File::userHomeDirectory),
                               "*.json");
        if (myChooser.browseForFileToOpen()) {
            File configFile (myChooser.getResult());
            hVst->setLastJSONDir(configFile.getParentDirectory());
            hVst->loadConfiguration (configFile);
        }
        //[/UserButtonCode_tb_loadJSON]
    }
    else if (buttonThatWasClicked == tb_saveJSON.get())
    {
        //[UserButtonCode_tb_saveJSON] -- add your button handler code here..
        FileChooser myChooser ("Save configuration...",
                               hVst->getLastJSONDir().exists() ? hVst->getLastJSONDir() : File::getSpecialLocation (File::userHomeDirectory),
                               "*.json");
        if (myChooser.browseForFileToSave (true)) {
            File configFile (myChooser.getResult());
            hVst->setLastJSONDir(configFile.getParentDirectory());
            hVst->saveConfigurationToFile (configFile);
        }
        //[/UserButtonCode_tb_saveJSON]
    }
    else if (buttonThatWasClicked == tb_render.get())
    {
        //[UserButtonCode_tb_render] -- add your button handler code here..
        if(!RenderingInProgress){
            try{
                std::thread render_thread(hosirrlib_render, hHS);
                render_thread.detach();
            } catch (const std::exception& exception) {
                std::cout << "Could not create thread to render" << exception.what() << std::endl;
            }
        }

        //[/UserButtonCode_tb_render]
    }
    else if (buttonThatWasClicked == tb_saveRIR.get())
    {
        //[UserButtonCode_tb_saveRIR] -- add your button handler code here..

        if(hosirrlib_getLsRIRstatus(hHS)==LS_RIR_STATUS_RENDERED){
            /* ask user, where to save */
            File outputFile;
            FileChooser myChooser ("Save configuration...",
                                   hVst->getSaveWavDirectory().exists() ? hVst->getSaveWavDirectory() : File::getSpecialLocation (File::userHomeDirectory),
                                    "*.wav");
            if (myChooser.browseForFileToSave (true)) {
                outputFile = (myChooser.getResult());
                hVst->setSaveWavDirectory(outputFile.getParentDirectory());

                /* fill audio buffer */
                AudioBuffer<float> buffer;
                buffer.setSize(hosirrlib_getNumLoudspeakers(hHS), hosirrlib_getAmbiRIRlength_samples(hHS));
                buffer.clear();
                float** lsRIR = buffer.getArrayOfWritePointers();
                hosirrlib_getLsRIR(hHS, lsRIR);

                /* write audio buffer to disk */
                WavAudioFormat wavFormat;
                std::unique_ptr<AudioFormatWriter> writer;
                outputFile.deleteFile();
                writer.reset (wavFormat.createWriterFor (new FileOutputStream (outputFile), (double)hosirrlib_getAmbiRIRsampleRate(hHS),
                                                         (unsigned int)hosirrlib_getNumLoudspeakers(hHS), 24, {}, 0));
                if (writer != nullptr)
                    writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
                writer = nullptr;
            }
         }

        //[/UserButtonCode_tb_saveRIR]
    }
    else if (buttonThatWasClicked == tb_BroadBand1stPeak.get())
    {
        //[UserButtonCode_tb_BroadBand1stPeak] -- add your button handler code here..
        hosirrlib_setBroadBandFirstPeakFLAG(hHS, tb_BroadBand1stPeak->getToggleState());
        //[/UserButtonCode_tb_BroadBand1stPeak]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void PluginEditor::timerCallback()
{
    /* parameters whos values can change internally should be periodically refreshed */
    outputCoordsView_handle->setNCH(hosirrlib_getNumLoudspeakers(hHS));
    SL_num_loudspeakers->setValue(hosirrlib_getNumLoudspeakers(hHS),dontSendNotification);
    CBchFormat->setSelectedId(hosirrlib_getChOrder(hHS), dontSendNotification);
    CBnormScheme->setSelectedId(hosirrlib_getNormType(hHS), dontSendNotification);
    CBchFormat->setItemEnabled(CH_FUMA, hosirrlib_getAnalysisOrder(hHS)==ANALYSIS_ORDER_FIRST ? true : false);
    CBnormScheme->setItemEnabled(NORM_FUMA, hosirrlib_getAnalysisOrder(hHS)==ANALYSIS_ORDER_FIRST ? true : false);
    label_inputOrder->setText(String(hosirrlib_getAmbiRIRinputOrder(hHS)), dontSendNotification);
    label_inputLength->setText(String(hosirrlib_getAmbiRIRlength_seconds(hHS)), dontSendNotification);
    label_inputSampleRate->setText(String(hosirrlib_getAmbiRIRsampleRate(hHS)), dontSendNotification);
    SL_windowSize->setValue(hosirrlib_getWindowLength(hHS), dontSendNotification);
    CBanaOrder->setSelectedId(hosirrlib_getAnalysisOrder(hHS), dontSendNotification);
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



//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PluginEditor" componentName=""
                 parentClasses="public AudioProcessorEditor, public Timer, private FilenameComponentListener"
                 constructorParams="PluginProcessor* ownerFilter" variableInitialisers="AudioProcessorEditor(ownerFilter), progressbar(progress), fileChooser (&quot;File&quot;, File(), true, false, false, &quot;*.wav&quot;, String(),&#10; &quot;Load *.wav File&quot;), thumbnailCache (5)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="656" initialHeight="380">
  <BACKGROUND backgroundColour="ffffffff">
    <RECT pos="0 192 656 188" fill="linear: 8 384, 8 312, 0=ff1c3949, 1=ff071e22"
          hasStroke="0"/>
    <RECT pos="0 30 656 163" fill="linear: 8 32, 8 88, 0=ff1c3949, 1=ff071e22"
          hasStroke="0"/>
    <RECT pos="12 119 213 81" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <ROUNDRECT pos="1 1 654 32" cornerSize="5.0" fill="linear: 0 32, 656 24, 0=ff061c20, 1=ff1c3949"
               hasStroke="1" stroke="2, mitered, butt" strokeColour="solid: ffb9b9b9"/>
    <RECT pos="0 0 656 2" fill="solid: 61a52a" hasStroke="1" stroke="2, mitered, butt"
          strokeColour="solid: ffb9b9b9"/>
    <RECT pos="12 88 213 32" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="12 58 213 32" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 35a0a0a0"/>
    <RECT pos="446 153 196 215" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="230 89 210 111" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="446 90 196 64" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <TEXT pos="455 90 67 30" fill="solid: ffffffff" hasStroke="0" text="Presets: "
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="499 154 105 28" fill="solid: ffffffff" hasStroke="0" text="Azi&#176;   #   Elev&#176;"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="455 118 169 30" fill="solid: ffffffff" hasStroke="0" text="N Loudspeakers"
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="48 34 144 30" fill="solid: ffffffff" hasStroke="0" text="Input Ambi RIR"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="36" typefaceStyle="Bold"/>
    <TEXT pos="472 33 144 30" fill="solid: ffffffff" hasStroke="0" text="Output Lspkr RIR"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="36" typefaceStyle="Bold"/>
    <TEXT pos="20 89 132 30" fill="solid: ffffffff" hasStroke="0" text="Format:"
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <RECT pos="12 224 428 144" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <TEXT pos="16 1 480 32" fill="solid: ffffffff" hasStroke="0" text="Higher-Order Spatial Impulse Response Renderer"
          fontname="Default font" fontsize="18.8" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <RECT pos="0 0 2 380" fill="solid: 61a52a" hasStroke="1" stroke="2, mitered, butt"
          strokeColour="solid: ffb9b9b9"/>
    <RECT pos="654 0 2 380" fill="solid: 61a52a" hasStroke="1" stroke="2, mitered, butt"
          strokeColour="solid: ffb9b9b9"/>
    <RECT pos="0 378 656 2" fill="solid: 61a52a" hasStroke="1" stroke="2, mitered, butt"
          strokeColour="solid: ffb9b9b9"/>
    <TEXT pos="236 88 132 30" fill="solid: ffffffff" hasStroke="0" text="Analysis Order:"
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="236 116 132 30" fill="solid: ffffffff" hasStroke="0" text="Window Length: "
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="236 144 193 31" fill="solid: ffffffff" hasStroke="0" text="Wet/Dry:"
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="236 171 188 33" fill="solid: ffffffff" hasStroke="0" text="Broad-Band First Peak: "
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="20 121 132 30" fill="solid: ffffffff" hasStroke="0" text="RIR order: "
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="20 145 132 30" fill="solid: ffffffff" hasStroke="0" text="RIR length (s): "
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="20 169 132 30" fill="solid: ffffffff" hasStroke="0" text="RIR Samplerate: "
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="300 33 93 30" fill="solid: ffffffff" hasStroke="0" text="Renderer"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="176 199 108 30" fill="solid: ffffffff" hasStroke="0" text="RIR Display"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="36" typefaceStyle="Bold"/>
    <TEXT pos="132 223 132 30" fill="solid: ffffffff" hasStroke="0" text="Time Trim:"
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="305 223 132 30" fill="solid: ffffffff" hasStroke="0" text="Scale:"
          fontname="Default font" fontsize="14.5" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <RECT pos="12 58 213 31" fill="solid: 8f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 35a0a0a0"/>
    <RECT pos="230 58 210 31" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 35a0a0a0"/>
    <RECT pos="230 58 210 31" fill="solid: 8f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 35a0a0a0"/>
    <RECT pos="446 58 196 32" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="446 58 196 32" fill="solid: 8f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
  </BACKGROUND>
  <COMBOBOX name="new combo box" id="5a2f99f88aa51390" memberName="CBoutputDirsPreset"
            virtualName="" explicitFocusOrder="0" pos="520 96 112 20" editable="0"
            layout="33" items="Default" textWhenNonSelected="Default" textWhenNoItems="(no choices)"/>
  <SLIDER name="new slider" id="2c2a2b3d0614cc94" memberName="SL_num_loudspeakers"
          virtualName="" explicitFocusOrder="0" pos="592 124 40 20" min="1.0"
          max="64.0" int="1.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <COMBOBOX name="new combo box" id="a36915795f16ceb6" memberName="CBchFormat"
            virtualName="" explicitFocusOrder="0" pos="77 95 68 20" editable="0"
            layout="33" items="" textWhenNonSelected="ACN" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="new combo box" id="e10be54628a6df43" memberName="CBnormScheme"
            virtualName="" explicitFocusOrder="0" pos="147 95 72 20" editable="0"
            layout="33" items="" textWhenNonSelected="N3D" textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="new button" id="527e24c6748d02d4" memberName="tb_loadJSON"
              virtualName="" explicitFocusOrder="0" pos="456 161 34 14" bgColOff="ff14889e"
              buttonText="Import" connectedEdges="2" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="48c5d3526dcfe64f" memberName="tb_saveJSON"
              virtualName="" explicitFocusOrder="0" pos="600 161 34 14" bgColOff="ff224d97"
              bgColOn="ff181f9a" buttonText="Export" connectedEdges="1" needsCallback="1"
              radioGroupId="0"/>
  <COMBOBOX name="new combo box" id="bb2f041afe2c8ed6" memberName="CBanaOrder"
            virtualName="" explicitFocusOrder="0" pos="344 95 88 20" editable="0"
            layout="33" items="" textWhenNonSelected="Default" textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="new button" id="b7db7350ba7d784f" memberName="tb_render"
              virtualName="" explicitFocusOrder="0" pos="264 65 144 18" bgColOff="ff9e8c14"
              buttonText="Render" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="dcf176a177d0d3a1" memberName="tb_saveRIR"
              virtualName="" explicitFocusOrder="0" pos="480 65 136 18" bgColOff="ff9e2f14"
              buttonText="Save" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="c59fb2aab2496c4e" memberName="label_inputOrder"
         virtualName="" explicitFocusOrder="0" pos="130 128 88 20" outlineCol="68a3a2a2"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="232f2ef4484843e8" memberName="label_inputLength"
         virtualName="" explicitFocusOrder="0" pos="130 152 88 20" outlineCol="68a3a2a2"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="63bb4c290bf063e2" memberName="label_inputSampleRate"
         virtualName="" explicitFocusOrder="0" pos="130 176 88 20" outlineCol="68a3a2a2"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="445631cc3238eb1" memberName="SL_wetDryBalance"
          virtualName="" explicitFocusOrder="0" pos="312 151 120 20" bkgcol="ff5c5d5e"
          trackcol="ff315b6d" min="0.0" max="2.0" int="0.01" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="new slider" id="c4e6dde9efca0e47" memberName="SL_windowSize"
          virtualName="" explicitFocusOrder="0" pos="360 122 71 20" min="32.0"
          max="256.0" int="2.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <COMBOBOX name="new combo box" id="d8e3211c7ae0fe1c" memberName="CBdisplayRIR"
            virtualName="" explicitFocusOrder="0" pos="21 229 99 18" editable="0"
            layout="33" items="" textWhenNonSelected="Ambi RIR" textWhenNoItems="(no choices)"/>
  <SLIDER name="new slider" id="22f1f1a1ab052e72" memberName="SL_displayGain"
          virtualName="" explicitFocusOrder="0" pos="344 229 88 20" bkgcol="ff5c5d5e"
          trackcol="ff315b6d" min="-24.0" max="24.0" int="0.01" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="new slider" id="e0aeb93b82919a24" memberName="SL_displayTimeTrim"
          virtualName="" explicitFocusOrder="0" pos="208 229 88 20" bkgcol="ff5c5d5e"
          trackcol="ff315b6d" min="0.01" max="1.0" int="0.01" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <TOGGLEBUTTON name="new toggle button" id="b87fbd348008eaa8" memberName="tb_BroadBand1stPeak"
                virtualName="" explicitFocusOrder="0" pos="410 174 26 24" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

