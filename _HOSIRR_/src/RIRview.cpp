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

#include "RIRview.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
RIRview::RIRview (int sourceSamplesPerThumbnailSample, AudioFormatManager& formatManager, AudioThumbnailCache& cache, int width, int height)
    : thumbnail (sourceSamplesPerThumbnailSample, formatManager, cache)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    labels = RIR_VIEW_SH_LABELS;
    compHeight = height;
    compWidth = width;
    setBounds(0,0,compWidth, compHeight);
    thumbnail.addChangeListener (this);
    //[/Constructor]
}

RIRview::~RIRview()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void RIRview::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded (g);
    else
        paintIfFileLoaded (g);
    //[/UserPaint]
}

void RIRview::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RIRview" componentName=""
                 parentClasses="public Component, private ChangeListener" constructorParams="int sourceSamplesPerThumbnailSample, AudioFormatManager&amp; formatManager, AudioThumbnailCache&amp; cache, int width, int height"
                 variableInitialisers="thumbnail (sourceSamplesPerThumbnailSample, formatManager, cache)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

